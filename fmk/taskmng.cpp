/******************************************************************************
 * File:    taskmng.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.TaskManager
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
 *   Implement TaskManager class
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

#include "taskmng.h"
#include "log.h"
#include "taskagent.h"
#include "tools.h"
#include "filetools.h"
#include "prodloc.h"
#include "str.h"
#include "dwatcher.h"

#include <fstream>

#include "json.hpp"
using json = nlohmann::json;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskManager::TaskManager(Config & _cfg, string _id, 
                         WorkArea & _wa, ProcessingNetwork & _net)
    : cfg(_cfg), id(_id), wa(_wa), net(_net),
      defaultProcCfg(std::string("sample.cfg.json")),
      logger(Log::getLogger("tskmng"))
{
    thisNodeNum = indexOf<string>(net.nodeName, id);
    logger.info("Task Manager created");

    setDirectoryWatchers();
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
TaskManager::~TaskManager()
{
    terminate();
}

//----------------------------------------------------------------------
// Method: setDirectoryWatchers
//----------------------------------------------------------------------
void TaskManager::setDirectoryWatchers()
{
    dirWatchers.push_back(DirWatchedAndQueue(new DirWatcher(wa.localOutputs),
                                             outboxProdQueue));
}

//----------------------------------------------------------------------
// Method: getNewEntries
//----------------------------------------------------------------------
bool TaskManager::getNewEntries()
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
bool TaskManager::getNewEntriesFromDirWatcher(DirWatcher * dw, Queue<string> & q)
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
// Method: createAgent
//----------------------------------------------------------------------
void TaskManager::createAgent(string id, WorkArea wa,
                 Queue<string> * iq, Queue<string> * oq, Queue<string> * tq,
                 bool isComm)
{
    TaskAgent * agent = new TaskAgent(wa, id, iq, oq, tq, isComm);
    agents.push_back(agent);
    agentThreads.push_back(std::thread(&TaskAgent::run, agent));
}

//----------------------------------------------------------------------
// Method: createAgents
//----------------------------------------------------------------------
void TaskManager::createAgents()
{
    vector<string> & thisNodeAgentNames = net.nodeAgents[id];
    numOfAgents = net.nodeNumOfAgents[thisNodeNum];

    logger.info("Creating %d processing agents for node %s. . .",
                numOfAgents, id.c_str());

     json agentData = {{"num_tasks", 0},
                       {"task_id", std::string("")},
                       {"cont_id", std::string("")},
                       {"cont_status", -99},
                       {"spectrum", {{"ABORTED", 0},
                                     {"ARCHIVED", 0},
                                     {"FAILED", 0},
                                     {"FINISHED", 0},
                                     {"PAUSED", 0},
                                     {"RUNNING", 0},
                                     {"SCHEDULED", 0},
                                     {"STOPPED", 0}}}};
    
    json agentsData;
    json agentsNames;
    json agentsTasks;

    for (int i = 0; i < numOfAgents; ++i) {
        Queue<string> * iq = new Queue<string>;
        Queue<string> * oq = new Queue<string>;
        Queue<string> * tq = new Queue<string>;
        string agName = thisNodeAgentNames.at(i);
        createAgent(agName, wa, iq, oq, tq, net.thisIsCommander);
        logger.debug("Creating agent " + std::to_string(i + 1) + " of " +
                     std::to_string(numOfAgents) + " :  " + agName);
        agentsInQueue.push_back(iq);
        agentsOutQueue.push_back(oq);
        agentsTskQueue.push_back(tq);
        agentsContainer[agName] = std::make_tuple(std::string(""), 
                                                  int(TaskStatus(TASK_UNKNOWN_STATE)));

        agentsData[agName] = agentData;

        agentsNames.push_back(agName);
        agentsTasks.push_back(0);

        AgentSpectrum sp;
        sp["ABORTED"]   = 0;
        sp["ARCHIVED"]  = 0;
        sp["FAILED"]    = 0;
        sp["FINISHED"]  = 0;
        sp["PAUSED"]    = 0;
        sp["RUNNING"]   = 0;
        sp["SCHEDULED"] = 0;
        sp["STOPPED"]   = 0;

        ai.agents.emplace(agName, AgentData({0, std::string(""), std::string(""), 
                        TASK_UNKNOWN_STATE, sp}));
        ai.agent_names.push_back(agName);
        ai.agent_num_tasks.push_back(0);
    }

//     std::cerr << ai.str() << '\n';
//     std::cerr << agentsInfo.dump() << '\n';

    agentsInfo["agents"] = agentsData;
    agentsInfo["agent_names"] = agentsNames;
    agentsInfo["agent_num_tasks"] = agentsTasks;
}

//----------------------------------------------------------------------
// Method: createTaskId
//----------------------------------------------------------------------
string TaskManager::createTaskId(string tskAgId, int n)
{
    static const char tplTaskId[] = "%s_%s-%04d";
    char buf[200];
    sprintf(buf, tplTaskId, tskAgId.c_str(), timeTag().c_str(), n);
    return std::string(buf);
}

//----------------------------------------------------------------------
// Method: createTaskFolders
//----------------------------------------------------------------------
void TaskManager::createTaskFolders(string & tskWkDir)
{
    vector<string> pths ;
    for (auto & p: {tskWkDir, tskWkDir + "/in", tskWkDir + "/out", tskWkDir + "/log"}) {
        if (mkdir(p.c_str(), PathMode) < 0) {
            logger.error("Couldn't create folder %s: %s",
                         p.c_str(), strerror(errno));
        }
    }        
}

//----------------------------------------------------------------------
// Method: createTask
// Create task for a given product
//----------------------------------------------------------------------
std::tuple<string, string> TaskManager::createTask(ProductMeta & meta, string & tskAgId,
                                                   int n, string & processor)
{
    // Create task id. and folder
    string taskId = createTaskId(tskAgId, n);
    string taskFld = wa.tasks + "/" + taskId;
    createTaskFolders(taskFld);
    
    // Place product in task input folder
    ProductLocator::toTaskInput(meta, wa, taskId);

    // Prepare environment for the execution of the processor
    string srcCfgProd = wa.procArea + "/" + processor + "/" + defaultProcCfg;
    string tgtCfgProd = taskFld + "/" + processor + ".cfg";
    FileTools::copyfile(srcCfgProd, tgtCfgProd);

    return std::tuple<string, string>(taskId, taskFld);
}

//----------------------------------------------------------------------
// Method: selectAgent
// Select an agent from the pool in this node as task responsible
//----------------------------------------------------------------------
std::tuple<int, int> TaskManager::selectAgent()
{
    json jNumOfTasks = agentsInfo["agent_num_tasks"];
    int ntasks = jNumOfTasks[0].get<int>();
    int nidx = 0;
    for (int i = 1; i < jNumOfTasks.size(); ++i) {
        int ntasksi = jNumOfTasks[i].get<int>();
        if (ntasks > ntasksi) { ntasksi = ntasks, nidx = i; }
    }
    return std::tuple<int, int>(nidx, ntasks);
}

//----------------------------------------------------------------------
// Method: updateAgent
// Update agents information structure
//----------------------------------------------------------------------
void TaskManager::updateAgent(string & taskId, int agNum,
                              string & agName, int agNumTsk)
{
    agentsInfo["agent_num_tasks"][agNum] = agNumTsk;
    json & agInfo = agentsInfo["agents"][agName];
    agInfo["task_id"] = taskId;
    agInfo["num_tasks"] = agNumTsk;
    agentsInfo["agents"][agName] = agInfo;
}

//----------------------------------------------------------------------
// Method: updateContainer
// Update container status in agent information structure
//----------------------------------------------------------------------
void TaskManager::updateContainer(string & agName, string contId,
                                  TaskStatus contStatus)
{
    json & agInfo = agentsInfo["agents"][agName];
    agInfo["cont_id"] = contId;
    agInfo["cont_status"] = int(contStatus);
    json & agInfoSpec = agInfo["spectrum"];
    
    string storedContId;
    int storedStatusVal;
    std::tie(storedContId, storedStatusVal) = agentsContainer[agName];
    TaskStatus storedContStatus(storedStatusVal);
    string storedContStatusStr = storedContStatus.str();
    int count = 0;
    bool newCont = storedContId.empty();

    if (newCont) {
        logger.info("Container %s launched, current status is %s",
                    contId.c_str(), contStatus.str().c_str());
    } else {
        count = agInfoSpec[storedContStatusStr].get<int>();
        agInfoSpec[storedContStatusStr] = count - 1;
        if (storedContStatus != contStatus) {
            logger.debug("Container %s changed from (%d) %s to (%d) %s",
                         contId.c_str(),
                         storedStatusVal,
                         storedContStatus.str().c_str(),
                         int(contStatus),
                         contStatus.str().c_str());
        }
    }
    
    agentsContainer[agName] = std::make_tuple(contId, int(contStatus));
    string newStatusStr = contStatus.str();
    count = agInfoSpec[newStatusStr].get<int>();
    agInfoSpec[newStatusStr] = count + 1;
}

//----------------------------------------------------------------------
// Method: updateTasksInfo
// Update task info in task queue
//----------------------------------------------------------------------
void TaskManager::updateTasksInfo(DataManager & datmng)
{
    int numOfAgents = net.thisNodeNumOfAgents;
    for (int agNum = 0; agNum < numOfAgents; ++agNum) {
        Queue<string> * tq = agentsTskQueue.at(agNum);
        string agName = agentsInfo["agent_names"][agNum];
        string justCreated, taskId, contId, inspect, percent, status;
        while (tq->get(justCreated)) {
            tq->get(taskId);
            tq->get(contId);
            tq->get(inspect);
            tq->get(percent);
            tq->get(status);
            int statusVal = TaskStatusVal[status];
            updateContainer(agName, contId, statusVal);
	    if (!inspect.empty()) {
		datmng.storeTaskInfo(taskId, statusVal,
				     inspect, justCreated == "true");
	    }
        }
    }
}

//----------------------------------------------------------------------
// Method: schedule
// Prepare task and send to selected agent
//----------------------------------------------------------------------
void TaskManager::schedule(ProductMeta & meta, string & processor)
{
    int agNum, numTasks;
    std::tie<int, int>(agNum, numTasks) = selectAgent();
    string agName = agentsInfo["agent_names"][agNum].get<std::string>();
    numTasks++;

    // Create task id and environment
    string taskId, taskFolder;
    std::tie<string, string>(taskId, taskFolder) =
        createTask(meta, agName, numTasks, processor);

    // Pass task id to selected agent
    Queue<string> * iq = agentsInQueue.at(agNum);
    iq->push(std::move(std::string(taskId)));
    iq->push(std::move(std::string(taskFolder)));
    iq->push(std::move(processor));

    // Update agents information structures
    updateAgent(taskId, agNum, agName, numTasks);
    //updateContainer(agName);
}

//----------------------------------------------------------------------
// Method: retrieveOutputs
// Returns the list of output products ready to be sent to the archive
//----------------------------------------------------------------------
void TaskManager::retrieveOutputs(Queue<string> & outputs)
{
    // Get new entries from Directory Watchers, and process them
    getNewEntries();
    
    ProductName prod;
    while (outboxProdQueue.get(prod)) {
        logger.debug("Registered output files: " + prod);
        outputs.push(std::move(prod));
    }
}

//----------------------------------------------------------------------
// Method: retrieveAgentsInfo
//----------------------------------------------------------------------
bool TaskManager::retrieveAgentsInfo(json & hi)
{
//     std::cerr << agentsInfo.dump() << '\n';

    vector<string> & nodeAgNames = net.nodeAgents[id];
    
    for (int agNum = 0; agNum < numOfAgents; ++agNum) {
        string serialAgName = nodeAgNames.at(agNum);
        Queue<string> * oq = agentsOutQueue.at(agNum);
        string agName;
        string msg;
        while (oq->get(agName)) {
            oq->get(msg);
            json & ags = agentsInfo["agents"];
            json & agThis = ags[agName];
            for (auto & el: str::split(msg, ' ')) {
                vector<string> parts = str::split(el, ':');
                agThis["spectrum"][parts.at(0)] = std::stoi(parts.at(1));
            }
        }
    }

    vector<double> loadavgs = getLoadAvgs();
    json loads(loadavgs);

    std::stringstream ss;
    std::ifstream fProcVersion("/proc/version");
    ss << fProcVersion.rdbuf();
    fProcVersion.close();
    string hostNameVersion = ss.str();
    hostNameVersion.pop_back();

    json machineInfo;
    machineInfo["load"] = loads;
    machineInfo["uname"] = hostNameVersion;

    agentsInfo["machine"] = machineInfo;

    hi = agentsInfo;
    return true;
}

//----------------------------------------------------------------------
// Method: showSpectra
//----------------------------------------------------------------------
void TaskManager::showSpectra()
{
    int numOfAgents = net.thisNodeNumOfAgents;
    for (int agNum = 0; agNum < numOfAgents; ++agNum) {
        string agName = agentsInfo["agent_names"][agNum];
        json & agInfo = agentsInfo["agents"][agName];
        string spectrum = agInfo["spectrum"].dump();
        logger.debug("Agent %d of %d - %s: %s", agNum + 1, numOfAgents,
                     agName.c_str(), spectrum.c_str());
    }
}

//----------------------------------------------------------------------
// Method: terminate
//----------------------------------------------------------------------
void TaskManager::terminate()
{
    for (auto & thr : agentThreads) { thr.join(); }
}

