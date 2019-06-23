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

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskManager::TaskManager(Config & _cfg, string _id, 
                         WorkArea & _wa, ProcessingNetwork & _net)
    : cfg(_cfg), id(_id), wa(_wa), net(_net),
      defaultProcCfg(std::string("cample.cfg.json")),
      logger(Log::getLogger("tskmng"))
{
    thisNodeNum = indexOf<string>(net.nodeName, id);
    logger.info("Task Manager created");
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

    logger.info("Creating processing %d agents for node %s. . .",
		numOfAgents, id);
    
    jso emptySpec;
    emptySpec.append("aborted", 0);
    emptySpec.append("archived", 0);
    emptySpec.append("failed", 0);
    emptySpec.append("finished", 0);
    emptySpec.append("paused", 0);
    emptySpec.append("running", 0);
    emptySpec.append("scheduled", 0);
    emptySpec.append("stopped", 0);

    jso agentData;
    agentData.append("num_tasks", 0);
    agentData.append("task_id", std::string(""));
    agentData.append("cont_id", std::string(""));
    agentData.append("cont_status", -99);
    agentData.append("spectrum", emptySpec);

    jso agentsData;
    jsa agentsNames;
    jsa agentsTasks;

    for (int i = 0; i < numOfAgents; ++i) {
	Queue<string> * iq = new Queue<string>;
	Queue<string> * oq = new Queue<string>;
	Queue<string> * tq = new Queue<string>;
	createAgent(thisNodeAgentNames.at(i), wa, iq, oq, tq, net.thisIsCommander);
	logger.debug("Creating agent " + std::to_string(i + 1) + " of " +
		     std::to_string(numOfAgents) + " :  " +
		     thisNodeAgentNames.at(i));
	agentsInQueue.push_back(iq);
	agentsOutQueue.push_back(oq);
	agentsTskQueue.push_back(tq);

	agentsData.append(thisNodeAgentNames.at(i), agentData);
	agentsNames.append(thisNodeAgentNames.at(i));
	agentsTasks.append(0);
    }

    agentsInfo.append("agents", agentsData);
    agentsInfo.append("agent_names", agentsNames);
    agentsInfo.append("agent_num_tasks", agentsTasks);
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
    jsa jNumOfTasks = agentsInfo["agent_num_tasks"].asArray();
    int ntasks = jNumOfTasks[0].asInt();
    int nidx = 0;
    for (int i = 1; i < jNumOfTasks.size(); ++i) {
	int ntasksi = jNumOfTasks[0].asInt();
	if (ntasks > ntasksi) { ntasksi = ntasks, nidx = i; }
    }
    return std::tuple<int, int>(nidx, ntasks);
}

//----------------------------------------------------------------------
// Method: updateAgent
//----------------------------------------------------------------------
void TaskManager::updateAgent(string & taskId, int agNum,
			      string & agName, int agNumTsk)
{
}

//----------------------------------------------------------------------
// Method: updateContainer
//----------------------------------------------------------------------
void TaskManager::updateContainer(string & agName, string & contId,
				  TaskStatus & contStatus)
{
}

//----------------------------------------------------------------------
// Method: updateTasksInfo
//----------------------------------------------------------------------
void TaskManager::updateTasksInfo(DataManager & datmng)
{
}

//----------------------------------------------------------------------
// Method: schedule
// Prepare task and send to selected agent
//----------------------------------------------------------------------
void TaskManager::schedule(ProductMeta & meta, string & processor)
{
    int numAg, numTasks;
    std::tie<int, int>(numAg, numTasks) = selectAgent();
    string agName = agentsInfo["agent_names"][numAg].asString();
    numTasks++;

    string taskId, taskFolder;
    std::tie<string, string>(taskId, taskFolder) = createTask(meta, agName,
							      numTasks, processor);
}

//----------------------------------------------------------------------
// Method: retrieveOutputs
//----------------------------------------------------------------------
void TaskManager::retrieveOutputs(Queue<string> & outputs)
{
}

//----------------------------------------------------------------------
// Method: retrieveAgentsInfo
//----------------------------------------------------------------------
bool TaskManager::retrieveAgentsInfo(js & hi)
{
    json::Object o;

    json::Parser jparser;

    vector<string> & nodeAgNames = net.nodeAgents[id];
    
    for (int agNum = 0; agNum < numOfAgents; ++agNum) {
	Queue<string> * oq = agentsOutQueue.at(agNum);
	string msg;
	while (oq->get(msg)) {
	    json::Object msgObj;
	    jparser.parse(msg, msgObj);
	    int k = msgObj["agId"].asInt();
	    js spec(msgObj["spectrum"]);
	    agentsInfo["agents"][nodeAgNames.at(agNum)]["spectrum"] = spec;
	}
    }

    vector<double> loadavgs = getLoadAvgs();
    jsa loads;
    for (double & d: loadavgs) { loads.append(d); }

    std::stringstream ss;
    std::ifstream fProcVersion;
    fProcVersion.open("/proc/version");
    ss << fProcVersion.rdbuf();
    fProcVersion.close();
    string hostNameVersion = ss.str();
    hostNameVersion.pop_back();

    jso machineInfo;
    machineInfo.append("load", loads);
    machineInfo.append("uname", hostNameVersion);

    agentsInfo.append("machine", machineInfo);

    hi.assign(agentsInfo);
    return true;
}

//----------------------------------------------------------------------
// Method: showSpectra
//----------------------------------------------------------------------
void TaskManager::showSpectra()
{
    int numOfAgents = net.thisNodeNumOfAgents;
    for (int agNum = 0; agNum < numOfAgents; ++agNum) {
	string agName = agentsInfo["agent_names"].asArray()[agNum].asString();
	jso agInfo = agentsInfo["agents"].asObject()[agName].asObject();
	string spectrum = agInfo["spectrum"].asObject().str();
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

