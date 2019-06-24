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
#include "fmt.h"

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
      logger(Log::getLogger(_id))
{
    // Read configuration
    JsonFileHandler jFile(cfgFileName);
    if (!jFile.read()) {
	logger.fatal("Cannot open config. file '" + cfgFileName + "'. Exiting.");
    }
    cfg = jFile.getData();

    // Initialize processing network variables
    net = new ProcessingNetwork(cfg, id, balanceMode);
    logger.info("Node " + id + " has " + std::to_string(net->thisNodeNumOfAgents) +
	    " processing agents");

    dist = new std::uniform_int_distribution<int>(0, net->numOfNodes - 1);
	
    masterLoopSleep_ms = cfg["general"]["masterHeartBeat"].asInt();
	
    // Create task orchestrator and manager
    tskOrc = new TaskOrchestrator(cfg, id);
    tskMng = new TaskManager(cfg, id, wa, *net);

    // Create Data Manager
    dataMng = (net->thisIsCommander) ? new DataManager(cfg) : nullptr;

    // Create HTTP server and requester object
    httpServer = new MasterServer(this, port, wa.serverBase);
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
	selectNodeFn = [](Master * m){ return 0; };
    }

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
	    // 	   std::string(e.name));
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
	return nodeInfo.asObject().str(); //nodeInfo.asObject().str();
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
    if (nodeInfoIsAvailable) {
	for (int i = 0; i < nodeStatus.size(); ++i) {
	    jsa jloads = nodeStatus[i]["machine"]["load"].asArray();
	    std::stringstream ss;
	    ss << "LOADS: " << jloads;
	    logger.debug(ss.str());
	    loads[i] = jloads[0].asFloat();
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
		logger.error("Cannot send file %s to node %s", prod, nodeToUse);
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
    while (productsForProcessing.get(prod)) {
	if (! checkIfProduct(prod, meta)) {
	    logger.debug(meta.str());
	    logger.warn("File '" + prod + "' doesn't seem to be a valid product");
	    continue;
	}
	logger.info("Product '" + prod + "' will be processed");

	if (!ProductLocator::toLocalArchive(meta, wa)) {
	    logger.error("Move (link) to archive of %s failed", prod);
	    continue;
	}

	if (! tskOrc->schedule(meta, *tskMng)) {
	    (void)unlink(prod.c_str());
	}
    }

    // The inputs products dispatched to other nodes only have to be
    // then archived in the local archive (in case this is the commander)
    if (net->thisIsCommander) {
	while (productsForArchival.get(prod)) {
	    (void)unlink(prod.c_str());
	}
    }	
}

//----------------------------------------------------------------------
// Method: archiveOutputs
//
//----------------------------------------------------------------------
void Master::archiveOutputs()
{
}

//----------------------------------------------------------------------
// Method: transferRemoteLocalArchiveToCommander
//
//----------------------------------------------------------------------
void Master::transferRemoteLocalArchiveToCommander()
{
}

//----------------------------------------------------------------------
// Method: transferOutputsToCommander
//
//----------------------------------------------------------------------
void Master::transferOutputsToCommander()
{
}

//----------------------------------------------------------------------
// Method: gatherNodesInfo
// Collects all nodes agents info
//----------------------------------------------------------------------
void Master::gatherNodesInfo()
{
    if (nodeStatus.size() < net->numOfNodes) {
    	nodeStatus.reserve(net->numOfNodes);
    }
    //nodeStatus.clear();
    for (auto & node: net->nodesButComm) {
	auto it = std::find(net->nodeName.begin(),
			    net->nodeName.end(), node);
	int i = it - net->nodeName.begin();
	httpRqstr->setServerUrl(net->nodeServerUrl.at(i));
	string resp;
	if (!httpRqstr->requestData("/status", resp)) {
	    logger.warn("Couldn't get node '%s' information from "
			"master commander", node.c_str());
	    nodeStatus.push_back(js(-1));
	    continue;
	}	    
	jso respObj;
	logger.debug("Response from %s: %s", node.c_str(), resp.c_str());
	json::Parser jParser;
	if (!jParser.parse(resp, respObj)) {
	    logger.warn("Problems in the translation of node '%s' "
			"information", node.c_str());
	    nodeStatus.push_back(js(-1));
	    continue;
	}
	nodeStatus.push_back(js(respObj));
    }
    //nodeInfoIsAvailable = true;
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
	    logger.debug("Node info retrieved: " + nodeInfo.asObject().str());
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
	    gatherNodesInfo();
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



 
