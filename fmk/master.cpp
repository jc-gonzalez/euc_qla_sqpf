/******************************************************************************
 * File:    master.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.Master
 *
 * Last update:  1.0
 *
 * Date:    20190614
 *
 * Author:  J C Gonzalez
 *
 * Copyright (C) 2019 Euclid SOC Team / J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement Master class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   TBD
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog> file
 *
 * About: License Conditions
 *   See <License> file
 *
 ******************************************************************************/

#include "master.h"

#include "alert.h"
#include "jsonfhdl.h"
#include "dwatcher.h"

#include <tuple>
#include <algorithm>
#include <random>
#include "limits.h"

#include "filetools.h"
#include "fnamespec.h"
#include "prodloc.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
Master::Master(string _cfg, string _id, int _port, string _wa, int _bMode)
    : cfgFileName(_cfg),
      id(_id),
      workArea(_wa),
      port(_port),
      balanceMode(_bMode),
      wa(WorkArea(_wa)),
      lastNodeUsed(0),
      nodeInfoIsAvailable(false),
      logger(Log::getLogger("master"))
{
    // Read configuration
    JsonFileHandler jFile(cfgFileName);
    if (!jFile.read()) {
        logger.fatal("Cannot open config. file '" + cfgFileName + "'. Exiting.");
    }
    cfg = jFile.getData();
    //logger.debug(cfg.dump());

    cfg["general"]["workArea"] = workArea;

    // Initialize processing network variables
    net = new ProcessingNetwork(cfg, id, balanceMode);
    logger.info("Node " + id + " has " + std::to_string(net->thisNodeNumOfAgents) +
            " processing agents");

    dist = new std::uniform_int_distribution<int>(0, net->numOfNodes - 1);

    masterLoopSleep_ms = cfg["general"]["masterHeartBeat"].get<int>();

    // Create task orchestrator and manager
    tskOrc = new TaskOrchestrator(cfg, id);
    tskMng = new TaskManager(cfg, id, wa, *net);

    // Create Data Manager
    if (net->thisIsCommander) {
        dataMng = new DataManager(cfg, *net);
        dataMng->initializeDB();
    } else {
        dataMng = nullptr;
    }

    // Create HTTP server and requester object
    httpServer = new MasterServer(this, tskMng, port, wa);
    httpRqstr = new MasterRequester;

    logger.info("HTTP Server started at port " + std::to_string(port) +
                " (" + wa.serverBase + ")");

    // Initialize loads
    loads = VFOR(1.0, x, net->nodeName);

    // Create node selection function
    switch (balanceMode) {
    case BalancingModeEnum::BALANCE_Sequential:
        selectNodeFn = [](Master * m){
            return (m->lastNodeUsed + 1) % m->net->numOfNodes; };
        break;
    case BalancingModeEnum::BALANCE_LoadBalance:
        selectNodeFn = [](Master * m){
            int i = 0, imin = 0;
            double minLoad = 999.;
            for (auto x : m->loads) {
                if (x < minLoad) { imin = i, minLoad = x; }
                ++i;
            }
            return imin; };
        break;
    case BalancingModeEnum::BALANCE_Random:
        selectNodeFn = [](Master * m){ return m->genRandomNode(); };
        break;
    default:
        selectNodeFn = [](Master * m){ return - m->balanceMode - 1; };
    }
    //selectNodeFn = [](Master * m){ return 1; };

    // Launch server
    httpServer->launch();

    // Define start session
    startSession();    
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
Master::~Master()
{
    terminate();
}

//----------------------------------------------------------------------
// Method: run
//
//----------------------------------------------------------------------
void Master::run()
{
    // Load State Vector
    loadStateVector();

    // Create Agents
    tskMng->createAgents();

    // Re-launch suspended tasks
    appendProdsToQueue( lookForSuspendedTasks() );

    // Create directory watchers
    setDirectoryWatchers();

    // Run main loop
    runMainLoop();

    // End
    terminate();
}

//----------------------------------------------------------------------
// Method: startSession
//
//----------------------------------------------------------------------
void Master::startSession()
{
    logger.info("Starting a new session with id " + wa.sessionId);
    logger.info("Session path is " + wa.sessionDir);
}

//----------------------------------------------------------------------
// Method: loadStateVector
//
//----------------------------------------------------------------------
void Master::loadStateVector()
{
}

//----------------------------------------------------------------------
// Method: lookForSuspendedTasks
//
//----------------------------------------------------------------------
vector<string> & Master::lookForSuspendedTasks()
{
    return productsFromSuspTasks;
}

//----------------------------------------------------------------------
// Method: appendProdsToQueue
//
//----------------------------------------------------------------------
void Master::appendProdsToQueue(vector<string> & prods)
{
    for (auto & fileName: prods) { productList.push(std::move(fileName)); }
    prods.clear();
}

//----------------------------------------------------------------------
// Method: appendProdsToQueue
//
//----------------------------------------------------------------------
void Master::appendProdsToQueue(Queue<string> & prods)
{
    std::string fileName;
    while (prods.get(fileName)) { productList.push(std::move(fileName)); }
}

//----------------------------------------------------------------------
// Method: setDirectoryWatchers
//
//----------------------------------------------------------------------
void Master::setDirectoryWatchers()
{
    dirWatchers.push_back(DirWatchedAndQueue(new DirWatcher(wa.reproc),
                                             reprocProdQueue));
    dirWatchers.push_back(DirWatchedAndQueue(new DirWatcher(wa.localInbox),
                                             inboxProdQueue));
}

//----------------------------------------------------------------------
// Method: getNewEntries
//----------------------------------------------------------------------
bool Master::getNewEntries()
{
    bool weHaveNewEntries = false;
    DirWatcher * dw;
    Queue<string> q;
    for (DirWatchedAndQueue grp : dirWatchers) {
        DirWatcher * dw = std::get<0>(grp);
        Queue<string> & q = std::get<1>(grp);
        weHaveNewEntries |= getNewEntriesFromDirWatcher(dw, q);
    }
    return weHaveNewEntries;
}

//----------------------------------------------------------------------
// Method: getNewEntriesFromDirWatcher
//----------------------------------------------------------------------
bool Master::getNewEntriesFromDirWatcher(DirWatcher * dw, Queue<string> & q)
{
    // Process new events, at most 5 per iteration
    static const int numMaxEventsPerIter = 5;

    DirWatcher::DirWatchEvent e;
    int numEvents = 0;
    while ((dw->nextEvent(e)) && (numEvents < numMaxEventsPerIter)) {
        logger.info("New DirWatchEvent: " + e.path + "/" + e.name
                + (e.isDir ? " DIR " : " ") + std::to_string(e.mask));

        // Process only files
        // TODO: Process directories that appear at inbox
        if (! e.isDir) {
            // Build full file name and add it to the queue
            q.push(fmt("$/$", e.path, e.name));
            ++numEvents;
        }
    }
    return (numEvents > 0);
}

//----------------------------------------------------------------------
// Method: getHostInfo
//
//----------------------------------------------------------------------
string Master::getHostInfo()
{
    if (nodeInfoIsAvailable) {
        return nodeInfo.dump(); //nodeInfo.dump();
    } else {
        return "{}";
    }
}

//----------------------------------------------------------------------
// Method: checkIfProduct
//
//----------------------------------------------------------------------
bool Master::checkIfProduct(string & fileName, ProductMeta & meta,
                            bool & needsVersion)
{
    static FileNameSpec fns;
    return fns.parse(fileName, meta, needsVersion);
}

//----------------------------------------------------------------------
// Method: distributeProducts
// Distribute products among all the procesing nodes
//----------------------------------------------------------------------
void Master::distributeProducts()
{
    for (int i = 0; i < nodeStatus.size(); ++i) {
        if (nodeStatusIsAvailable[i]) {
            try {
                //logger.info(std::to_string(i) + ": " + nodeStatus[i].dump());
                json jloads = nodeStatus[i]["machine"]["load"];
                //std::stringstream ss;
                //ss << "LOADS: " << jloads;
                //logger.debug(ss.str());
                loads[i] = jloads[0].get<double>();
            } catch (...) {
                loads[i] = 1.0;
            }
        }
    }

    ProductName prod;
    ProductMeta meta;
    bool needsVersion;

    while (productList.get(prod)) {
        int numOfNodeToUse = selectNodeFn(this);
        string nodeToUse = net->nodeName[numOfNodeToUse];
        bool processInThisNode = nodeToUse == id;
        
        if (! checkIfProduct(prod, meta, needsVersion)) {
            logger.warn("File '" + prod + "' doesn't seem to be a valid product");
            continue;
        } else {
            //logger.debug("META >>>>>> " + meta.dump());
            // If it is a JSON file, we assume it is a QLA report, so we will use the
            // current node to process it
            // In this case, the version will already be in the file name, so we
            // can skip next "if"
            // logger.info("Product to process: " + meta.dump());
            if ("JSON" == meta["format"].get<string>()) {
                numOfNodeToUse = net->commanderNum;
                nodeToUse = id;
                processInThisNode = true;
                break;
            }
            if (needsVersion) {
                string newVersion = dataMng->getNewVersionForSignature(meta["instance"]);
                json & fs = meta["fileinfo"];
                string folder = fs["path"].get<string>();
                string newName = (fs["sname"].get<string>() + "_" + newVersion +
                                  "." + fs["ext"].get<string>());
                string newProd = folder + "/" + newName;
                logger.debug("Changing name from " + prod + " to " + newProd);                
                //DirWatcher * dw = std::get<0>(dirWatchers.back());
                //dw->skip(newName, false); // !processInThisNode);
                if (rename(prod.c_str(), newProd.c_str()) != 0) {
                    logger.error("Couldn't add version tag to product " + prod);
                }
                //prod = newProd;
                //(void)checkIfProduct(prod, meta, needsVersion);
                // logger.info("Product to process: " + meta.dump());
                continue;
            }
        }

        logger.debug("Processing of " + prod + " will be done by node " + nodeToUse);

        if (! processInThisNode) {
            // If the processing node is not the commander (I'm the
            // commander in this function), dispatch it to the
            // selected node, and save it to remove it from the list
            httpRqstr->setServerUrl(net->nodeServerUrl[numOfNodeToUse]);
            if (httpRqstr->postFile("/inbox", prod,
                                     "application/octet-stream")) {
                productsForArchival.push(std::move(prod));
            } else {
                logger.error("Cannot send file %s to node %s",
                             prod.c_str(), nodeToUse.c_str());
                logger.warn("Product will be processed by master node");
                processInThisNode = true;
                numOfNodeToUse = net->commanderNum;
                nodeToUse = id;
            }
        }

        if (processInThisNode) {
            productsForProcessing.push(std::move(prod));
        }
        
        lastNodeUsed = numOfNodeToUse;
    }
}

//----------------------------------------------------------------------
// Method: scheduleProductsForProcessing
// Schedule products for processing in one of the proc. nodes
//----------------------------------------------------------------------
void Master::scheduleProductsForProcessing()
{
    if (net->thisIsCommander) {
        // Commander: Distribute among all the nodes
        distributeProducts();
    } else {
        // Proc.node: Process all the products in the list
        productsForProcessing.append(productList);
    }

    ProductName prod;
    ProductMeta meta;
    bool b;
    ProductMetaList products;
    
    // Take and handle the products in processing list
    while (productsForProcessing.get(prod)) {
        if (! checkIfProduct(prod, meta, b)) {
            logger.warn("File '" + prod + "' doesn't seem to be a valid product");
            continue;
        }

        logger.info("Product '" + prod + "' will be processed");

        logger.debug(fmt("$:$: Try to archive product $",
                         __FUNCTION__, __LINE__, prod));

        if (!ProductLocator::toLocalArchive(meta, wa)) {
            logger.error("Move (link) to archive of %s failed", prod.c_str());
            continue;
        }
       
        if (! tskOrc->schedule(meta, *tskMng)) {
            logger.error("Couldn't schedule the processing of %s", prod.c_str());
            (void)unlink(prod.c_str());
            continue;
        }

        products.push_back(meta);
    }

    // The inputs products dispatched to other nodes only have to be
    // then archived in the local archive (in case this is the commander)
    if (net->thisIsCommander) {
        while (productsForArchival.get(prod)) {
            if (! checkIfProduct(prod, meta, b)) {
                logger.warn("File '" + prod + "' doesn't seem to be a valid product");
                continue;
            }
            products.push_back(meta);
        }
        
        dataMng->storeProducts(products);

        for (auto & m: products) {
            string prod = m["fileinfo"]["full"].get<string>();
            logger.debug("Removing archived product %s", prod.c_str());
            (void)unlink(prod.c_str());
        }
    }
}

//----------------------------------------------------------------------
// Method: archiveOutputs
// Save output products to the local archive
//----------------------------------------------------------------------
void Master::archiveOutputs()
{
    ProductName prod;
    ProductMeta meta;
    ProductMetaList products;
    bool needsVersion;
    while (outputProducts.get(prod)) {
        if (checkIfProduct(prod, meta, needsVersion)) {
            products.push_back(meta);
            logger.debug("Moving output product " + prod + " to archive");

            logger.debug(fmt("$:$: Try to archive product $",
                             __FUNCTION__, __LINE__, prod));
            ProductLocator::toLocalArchive(meta, wa, ProductLocator::MOVE);
        } else {
            logger.warn("Found non-product file in local outputs folder: " + prod);
        }
    }
    if (products.size() > 0) {
        dataMng->storeProducts(products);
    }
}

//----------------------------------------------------------------------
// Method: transferRemoteLocalArchiveToCommander
// Transfer (POST) outputs to server/outputs end of commander server
//----------------------------------------------------------------------
void Master::transferRemoteLocalArchiveToCommander()
{
    Queue<string> archProducts;
    for (auto & s: FileTools::filesInFolder(wa.archive)) {
        string prod(s);
        archProducts.push(std::move(prod));
    }
    transferFilesToCommander(archProducts, "/outputs");
}

//----------------------------------------------------------------------
// Method: transferOutputsToCommander
// Transfer (POST) outputs to server/outputs end of commander server
//----------------------------------------------------------------------
void Master::transferOutputsToCommander()
{
    transferFilesToCommander(outputProducts, "/outputs");
}

//----------------------------------------------------------------------
// Method: transferFilesToCommander
// Transfer (POST) outputs to server/XXX end of commander server
//----------------------------------------------------------------------
void Master::transferFilesToCommander(Queue<string> & prodQueue,
                                      string route)
{
    ProductName prod;
    while (prodQueue.get(prod)) {
        logger.info("Transfer of product " + prod + " for archival");
        httpRqstr->setServerUrl(net->commanderUrl);
        if (!httpRqstr->postFile(route, prod,
                                 "application/octet-stream")) {
            logger.error("Cannot send file " + prod + " to " + net->commander);
            continue;
        } 
        unlink(prod.c_str());
    }
}

//----------------------------------------------------------------------
// Method: gatherNodesStatus
// Collects all nodes agents info
//----------------------------------------------------------------------
void Master::gatherNodesStatus()
{
    nodeStatus.clear();
    nodeStatusIsAvailable.clear();
    
    for (auto & node: net->nodesButComm) {
        auto it = std::find(net->nodeName.begin(),
                            net->nodeName.end(), node);
        int i = it - net->nodeName.begin();

        httpRqstr->setServerUrl(net->nodeServerUrl.at(i));
        string resp;
        if (!httpRqstr->requestData("/status", resp)) {
            logger.warn("Couldn't get node '%s' information from "
                        "master commander", node.c_str());
            nodeStatus.push_back(json{-1});
            nodeStatusIsAvailable.push_back(false);
            continue;
        }

        //logger.debug("NodeStatus gathered: " + node + " := " + resp);
        
        json respObj;
        try {
            respObj = json::parse(resp);
        } catch(...) {
            logger.warn("Problems in the translation of node '%s' "
                        "information", node.c_str());
            nodeStatus.push_back(json{-1});
            nodeStatusIsAvailable.push_back(false);
            continue;
        }
        
        nodeStatus.push_back(respObj);
        nodeStatusIsAvailable.push_back(true);
    }
}

//----------------------------------------------------------------------
// Method: gatherTasksStatus
// Collects all nodes tasks info
//----------------------------------------------------------------------
void Master::gatherTasksStatus()
{
    int i = -1;
    for (auto & node: net->nodeName) {
        ++i;
        httpRqstr->setServerUrl(net->nodeServerUrl.at(i));
        string resp;
        if (!httpRqstr->requestData("/tstatus", resp)) {
            logger.warn("Couldn't get node '%s' information from "
                        "master commander", node.c_str());
            continue;
        }

        if (resp == "{}") { continue; }
        
        json respObj;
        try {
            respObj = json::parse(resp);
        } catch(...) {
            logger.warn("Problems in the translation of tasks info. from node '%s'",
                        node.c_str());
            continue;
        }

        // Retrieve info for tasks on the node, and store it into DB
        for (auto & kv: respObj.items()) {
            string const & agName = kv.key();
            json const & agTaskInfo = kv.value();

            string tid = agTaskInfo["task_id"];
            string tinfo = agTaskInfo["info"].dump();
            string tstatus = agTaskInfo["status"];
            int statusVal = TaskStatusVal[tstatus];
            bool isNew = agTaskInfo["new"];
            //logger.debug("Storing Task " + tid + " (" + tstatus + ") " + (isNew ? "NEW" : ""));
            
            dataMng->storeTaskInfo(tid, statusVal, tinfo, isNew);
        }
    }
}

//----------------------------------------------------------------------
// Method: runMainLoop
//
//----------------------------------------------------------------------
void Master::runMainLoop()
{
    logger.info("Start!");
    int iteration = 1;

    // Get start time
    auto start_time = std::chrono::steady_clock::now();
    // Compute end time
    auto next_time = start_time + std::chrono::seconds(1);

    forever {

        logger.debug("Iteration " + std::to_string(iteration));

        // Collect new products to process
        if (getNewEntries()) {
            appendProdsToQueue(reprocProdQueue);
            appendProdsToQueue(inboxProdQueue);
        }

        // Schedule the processing
        if (!productList.empty()) {
            scheduleProductsForProcessing();
        }

        // Retrieve agents information
        if ((iteration == 1) || ((iteration % 5) == 0)) {
            nodeInfoIsAvailable = tskMng->retrieveAgentsInfo(nodeInfo);
            //logger.debug("Node info retrieved: " + nodeInfo.dump());
            tskMng->showSpectra();
        }

        // Retrieve pending outputs
        tskMng->retrieveOutputs(outputProducts);
        //outputProducts.dump();
        
        if (net->thisIsCommander) {
            // FOR COMMANDER

            // Place products in outputProducts list into the archive (and DB)
            archiveOutputs();
        } else {
            // FOR REMOTE NODES

            // 1. Takes all files in data/archive, and transfer them to commander
            //      REMOTE:data/archive  ==>  COMMANDER:server/outputs
            transferRemoteLocalArchiveToCommander();
            
            // 2. Transfer files declared as outputs to commander
            //      REMOTE:data/archive  ==>  COMMANDER:server/outputs
            transferOutputsToCommander();
        }

        // Update tasks information
        //if (net->thisIsCommander) {
        tskMng->updateTasksInfo();
        //}

        // Retrieve nodes information
        if ((net->thisIsCommander) &&
            ((iteration == 1) || ((iteration % 5) == 0))) {
            gatherNodesStatus();
            gatherTasksStatus();            
        }

        // Wait a little bit until next loop
        while (next_time < std::chrono::steady_clock::now()) {
            next_time += std::chrono::seconds(1);
            ++iteration;
        }
        std::this_thread::sleep_until(next_time);

    }
}

//----------------------------------------------------------------------
// Method: delay
// Waits for a small time lapse for system sync
//----------------------------------------------------------------------
void Master::delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//----------------------------------------------------------------------
// Method: terminate
// Delete handlers amnd cleanup
//----------------------------------------------------------------------
void Master::terminate()
{
    // Destroy all elements
    delete httpRqstr;
    delete httpServer;
    delete tskMng;
    delete tskOrc;
    if (net->thisIsCommander) delete dataMng;

    logger.info("Done.");
}

//----------------------------------------------------------------------
// Method: genRandomNode
// Generated the 0-base index of a Node, randomly
//----------------------------------------------------------------------
int Master::genRandomNode()
{
    return (*dist)(mt);
}

std::mt19937 Master::mt;

