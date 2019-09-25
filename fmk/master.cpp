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
    httpServer = new MasterServer(this, port, wa);
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
    DirWatcher::DirWatchEvent e;

    // Process new events, at most 5 per iteration
    int numMaxEventsPerIter = 5;
    int numEvents = 0;
    while ((dw->nextEvent(e)) && (numEvents < numMaxEventsPerIter)) {
        logger.info("New DirWatchEvent: " + e.path + "/" + e.name
                + (e.isDir ? " DIR " : " ") + std::to_string(e.mask));

        // Process only files
        // TODO: Process directories that appear at inbox
        if (! e.isDir) {
            // Build full file name and add it to the queue
            // q.push(std::string(e.path) + "/" +
            //            std::string(e.name));
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
bool Master::checkIfProduct(string & fileName, ProductMeta & meta)
{
    static FileNameSpec fns;
    return fns.parse(fileName, meta);
}

//----------------------------------------------------------------------
// Method: distributeProducts
// Distribute products among all the procesing nodes
//----------------------------------------------------------------------
void Master::distributeProducts()
{
    for (int i = 0; i < nodeStatus.size(); ++i) {
        if (nodeStatusIsAvailable[i]) { 
            logger.info(std::to_string(i) + ": " + nodeStatus[i].dump());
            json jloads = nodeStatus[i]["machine"]["load"];
            //std::stringstream ss;
            //ss << "LOADS: " << jloads;
            //logger.debug(ss.str());
            loads[i] = jloads[0].get<double>();
        }
    }

    ProductName prod;
    while (productList.get(prod)) {
        int numOfNodeToUse = selectNodeFn(this);
        string nodeToUse = net->nodeName[numOfNodeToUse];

        logger.debug("Processing of " + prod + " will be done by node " + nodeToUse);

        if (nodeToUse != id) {
            // If the node is not the commander (I'm the commander in
            // this function), dispatch it to the selected node, and
            // save it to remove it from the list
            httpRqstr->setServerUrl(net->nodeServerUrl[numOfNodeToUse]);
            if (!httpRqstr->postFile("/inbox", prod,
                                     "application/octet-stream")) {
                logger.error("Cannot send file %s to node %s",
                             prod.c_str(), nodeToUse.c_str());
                continue;
            } else {
                productsForArchival.push(std::move(prod));
            }
        } else {
            productsForProcessing.push(std::move(prod));
        }

        lastNodeUsed = numOfNodeToUse;
    }
}

//----------------------------------------------------------------------
// Method: scheduleProductsForProcessing
//
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

    // Process the products in one list
    ProductName prod;
    ProductMeta meta;
    ProductMetaList products;
    while (productsForProcessing.get(prod)) {
        if (! checkIfProduct(prod, meta)) {
            logger.debug(meta.dump());
            logger.warn("File '" + prod + "' doesn't seem to be a valid product");
            continue;
        }
        logger.info("Product '" + prod + "' will be processed");

        if (!ProductLocator::toLocalArchive(meta, wa)) {
            logger.error("Move (link) to archive of %s failed", prod.c_str());
            continue;
        }

        if (! tskOrc->schedule(meta, *tskMng)) {
            (void)unlink(prod.c_str());
        }

        products.push_back(meta);
    }

    // The inputs products dispatched to other nodes only have to be
    // then archived in the local archive (in case this is the commander)
    if (net->thisIsCommander) {
        dataMng->storeProducts(products);

        while (productsForArchival.get(prod)) {
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
    static FileNameSpec fns;

    ProductName prod;
    ProductMeta meta;
    ProductMetaList products;
    while (outputProducts.get(prod)) {
        if (fns.parse(prod, meta)) {
            products.push_back(meta);
            ProductLocator::toLocalArchive(meta, wa, ProductLocator::MOVE);
            logger.debug("Moving output product " + prod + " to archive");
        } else {
            logger.warn("Found non-product file in local outputs folder: " + prod);
        }
    }
    if (products.size() > 0) {
        logger.info("Found " + std::to_string(products.size()) + " products to archive");
        dataMng->storeProducts(products);
    } else {
        logger.warn("AAAARGH!! No products found to be archived!!");
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
        httpRqstr->setServerUrl(net->commanderUrl);
        if (!httpRqstr->postFile(route, prod,
                                 "application/octet-stream")) {
            logger.error("Cannot send file " + prod + " to " + net->commander);
            continue;
        } 
        logger.info("Transfer of product " + prod + " for archival");
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
// Method: runMainLoop
//
//----------------------------------------------------------------------
void Master::runMainLoop()
{
    logger.info("Start!");
    int iteration = 0;

    FileNameSpec fns;

    forever {

        ++iteration;
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
        if ((iteration == 1) || ((iteration % 10) == 0)) {
            nodeInfoIsAvailable = tskMng->retrieveAgentsInfo(nodeInfo);
            //logger.debug("Node info retrieved: " + nodeInfo.dump());
            tskMng->showSpectra();
        }

        // Retrieve pending outputs
        tskMng->retrieveOutputs(outputProducts);
        if (net->thisIsCommander) {
            archiveOutputs();
        } else {
            transferRemoteLocalArchiveToCommander();
            transferOutputsToCommander();
        }

        // Update tasks information
        if (net->thisIsCommander) {
            tskMng->updateTasksInfo(*dataMng);
        }

        // Retrieve nodes information
        if ((net->thisIsCommander) &&
            ((iteration == 1) || ((iteration % 20) == 0))) {
            gatherNodesStatus();
        }

        // Wait a little bit until next loop
        delay(masterLoopSleep_ms);

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
//
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

