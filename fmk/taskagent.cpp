/******************************************************************************
 * File:    taskagent.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.TaskAgent
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
 *   Implement TaskAgent class
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

#include "taskagent.h"
#include "str.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskAgent::TaskAgent(WorkArea & _wa, string _ident,
                     Queue<string> * _iq, Queue<string> * _oq, Queue<string> * _tq,
                     bool _isCommander)
    : wa(_wa), id(_ident), iq(_iq), oq(_oq), tq(_tq),
      isCommander(_isCommander),
      iAmQuitting(false),
      containerId(string("")),
      status(TASK_UNKNOWN_STATE),
      logger(Log::getLogger("tskag"))
{
    init();
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
TaskAgent::~TaskAgent()
{
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
void TaskAgent::init()
{
    // Create Container Manager
    dckMng = std::make_shared<ContainerMng>(ContainerMng(wa));
}

//----------------------------------------------------------------------
// Method: substitute
//----------------------------------------------------------------------
void TaskAgent::substitute(dict & rule)
{
}

//----------------------------------------------------------------------
// Method: get_substitution_rules
//----------------------------------------------------------------------
void TaskAgent::get_substitution_rules()
{
}

//----------------------------------------------------------------------
// Method: is_substitution_rules
//----------------------------------------------------------------------
void TaskAgent::is_substitution_rules()
{
}

//----------------------------------------------------------------------
// Method: stateToTaskStatus
// Class method to convert status/code to TaskStatus
//----------------------------------------------------------------------
TaskStatus TaskAgent::stateToTaskStatus(string inspStatus, int inspCode)
{
    if        (inspStatus == "running") {
	return TASK_RUNNING;
    } else if (inspStatus == "paused") {
	return TASK_PAUSED;
    } else if (inspStatus == "created") {
	return TASK_ABORTED;
    } else if (inspStatus == "dead") {
	return TASK_STOPPED;
    } else if (inspStatus == "exited") {
	if (inspCode == 0) {
	    return TASK_FINISHED;
	} else if ((inspCode > 128) && (inspCode < 160)) {
	    return iAmQuitting ? TASK_RUNNING : TASK_STOPPED;
	} else {
	    return TASK_FAILED;
	}
    } else {
	return TASK_UNKNOWN_STATE;
    }
}

//----------------------------------------------------------------------
// Method: isEnded
//----------------------------------------------------------------------
bool TaskAgent::isEnded(TaskStatus st)
{
    return ((st == TASK_STOPPED) ||
	    (st == TASK_FAILED) ||
	    (st == TASK_FINISHED));
}

//----------------------------------------------------------------------
// Method: do_rules
//----------------------------------------------------------------------
void TaskAgent::do_rules(dict item)
{
}

//----------------------------------------------------------------------
// Method: sendSpectrumToMng
//----------------------------------------------------------------------
void TaskAgent::sendSpectrumToMng()
{
}

//----------------------------------------------------------------------
// Method: prepareNewTask
//----------------------------------------------------------------------
bool TaskAgent::prepareNewTask(string taskId, string taskFld, string proc)
{
    return true;
}

//----------------------------------------------------------------------
// Method: launchContainer
//----------------------------------------------------------------------
bool TaskAgent::launchContainer(string & contId)
{
    return true;
}

//----------------------------------------------------------------------
// Method: inspectContainer
//----------------------------------------------------------------------
string TaskAgent::inspectContainer(string cntId, bool fullInfo, string filter)
{
    // Get inspect information
    std::stringstream info;
    bool ok = true;
    int iters = 0;
    do {
	info.str(fullInfo ? "" : filter); 
	ok = dckMng->getInfo(cntId, info);
	if (!ok) { delay(10); ++iters; }
	if (iters > 10) {
	    logger.error("Cannot inspect container with id %s", cntId.c_str());
	    return std::string("");
	}
    } while (! ok);

    return info.str();
}

//----------------------------------------------------------------------
// Method: launchNewTask
// Launch new task from the tasks pool (queue)
//----------------------------------------------------------------------
std::string TaskAgent::launchNewTask()
{
    string ntaskId, ntaskFolder, nprocessor;
    taskQueue.get(ntaskId);
    taskQueue.get(ntaskFolder);
    taskQueue.get(nprocessor);
    
    if (! prepareNewTask(ntaskId, ntaskFolder, nprocessor)) {
	return std::string("");
    }

    string contId("");
    if (launchContainer(contId)) {
	inspect = inspectContainer(contId);
	string statusLowStr(TaskStatusStr[TASK_RUNNING]);
	str::toLower(statusLowStr);
	for (auto & s : vector<string> {"true", taskId, inspect, "1", statusLowStr}) {
	    tq->push(std::move(s));
	}
	taskId = ntaskId;
	taskFolder = ntaskFolder;
	processor = nprocessor;
    }
    return contId;
}

//----------------------------------------------------------------------
// Method: scheduleContainerForRemoval
// Append container to list of containers to be removed.  This is done
// so that the removal is done some time after the container has exited.     
//----------------------------------------------------------------------
void TaskAgent::scheduleContainerForRemoval()
{
    containersToRemove.push_back(std::make_pair(clock(), containerId));
    logger.debug("Scheduling container %s for removal", containerId.c_str());
}

//----------------------------------------------------------------------
// Method: removeOldContainers
// Remove exited containers with an age of at least tOffset seconds
//----------------------------------------------------------------------
void TaskAgent::removeOldContainers()
{
}

//----------------------------------------------------------------------
// Method: atom_move
//----------------------------------------------------------------------
void TaskAgent::atom_move(string src, string dst)
{
}

//----------------------------------------------------------------------
// Method: prepareOutputs
//----------------------------------------------------------------------
void TaskAgent::prepareOutputs()
{
}

//----------------------------------------------------------------------
// Method: monitorTasks
// Monitor running container
//----------------------------------------------------------------------
void TaskAgent::monitorTasks()
{
    static string inspectSelection("{\"Id\":{{json .Id}},"
                                   "\"State\":{{json .State}},"
                                   "\"Path\":{{json .Path}},"
                                   "\"Args\":{{json .Args}}}");

    static json::Parser parser;
    string contId;
    
    // Check status of current container
    if (containerId.empty()) {
	if (taskQueue.empty()) { return; }

	contId = launchNewTask();
	if (contId.empty()) {
	    return;
	} else {
	    // Send information of new container
	    containerId = contId;
	    containerSpectrum.append(contId, "scheduled");
	}
    }
    
    contId = containerId;
    
    inspect = inspectContainer(contId);
    if (! inspect.empty()) {
	jso jinspect;
	parser.parse(inspect, jinspect);
	
	string inspStatus = jinspect["State"]["Status"].asString();
	int inspCode      = jinspect["State"]["ExitCode"].asInt();
	status = stateToTaskStatus(inspStatus, inspCode);
	string statusLowStr(TaskStatusStr[status]);
	str::toLower(statusLowStr);	
	for (auto & s : vector<string> {"false", taskId, inspect, "1", statusLowStr}) {
	    tq->push(std::move(s));
	}
	
	containerSpectrum.append(contId, statusLowStr);
    }
    
    sendSpectrumToMng();
    
    // If finished, set current container to None
    // (TBD: and remove info from internal lists and dicts)
    if (isEnded(status)) {
	prepareOutputs();
	scheduleContainerForRemoval();
	containerId = "";
    }
}

//----------------------------------------------------------------------
// Method: delay
// Waits for a small time lapse for system sync
//----------------------------------------------------------------------
void TaskAgent::delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
void TaskAgent::run()
{
    forever {

	// Check if new task data available
	if (! iq->empty()) {
	    // Gather new task info from input queue, and store in internal queue
	    string itaskId, itaskFolder, iprocessor;
	    iq->get(itaskId);
	    iq->get(itaskFolder);
	    iq->get(iprocessor);
	    
	    logger.debug("New task id queued at Task Agent %s: %s", id.c_str(), itaskId.c_str());
	    logger.debug("Execution to be done in work. dir. %s", itaskFolder.c_str());
	    logger.debug("Processor to use: %s", iprocessor.c_str());
	    taskQueue.push(std::move(itaskId));
	    taskQueue.push(std::move(itaskFolder));
	    taskQueue.push(std::move(iprocessor));
	}
	
	// Monitor running container
	monitorTasks();
	
	// Remove old containers
	removeOldContainers();
	
	// Minor sleep
	delay(333);
    }
}

