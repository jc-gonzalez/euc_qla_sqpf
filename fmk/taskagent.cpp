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
#include "fnamespec.h"
#include "filetools.h"
#include "prodloc.h"

#include <unistd.h>

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
// Method: getubstitutionRules
// Get the list of substitution rules
//----------------------------------------------------------------------
std::tuple<string, string> TaskAgent::getSubstitutionRules(string item)
{
    auto v = str::split(item.substr(1, item.length()-2), ':');
    return std::make_tuple(v.at(0), v.at(1));
}

//----------------------------------------------------------------------
// Method: isSubstitutionRules
// Determine if a certain item has rules inside
//----------------------------------------------------------------------
bool TaskAgent::isSubstitutionRules(string item)
{
    if (item.length() < 1) { return false; }
    return (item[0] == '{') && (item[item.length() - 1] == '}');
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
// Method: doRules
// Class method to apply a set of rules from config file
//----------------------------------------------------------------------
vector<string> TaskAgent::doRules(string item)
{
    string from_var, rules;
    std::tie(from_var, rules) = getSubstitutionRules(item);

    string value
        value = ' '.join(self.__dict__['p_' + from_var])
        #logger.debug(self.I + "Original value: {}".format(value))
        for rule in rules:
            #logger.debug(self.I + "{} : {}".format(value, rule))
            value = TaskAgent.substitute(value, rule)
        return value.split(' ')
}

//----------------------------------------------------------------------
// Method: sendSpectrumToMng
// Send message with container id. and status back to manager
//----------------------------------------------------------------------
void TaskAgent::sendSpectrumToMng()
{
    // Send information of the container
    string msgSpec;
    for (auto & kv: containerSpectrum.spectrum()) {
	msgSpec += kv.first + ":" + std::to_string(kv.second) + " ";
    }
    oq->push(string(id));
    oq->push(std::move(msgSpec));
}

//----------------------------------------------------------------------
// Method: prepareNewTask
// Prepare environment to launch new container for new task
//----------------------------------------------------------------------
bool TaskAgent::prepareNewTask(string taskId, string taskFld, string proc)
{
    // Read processor config. file
    string cfgFile(fmt("$/$.cfg", taskFld, proc));
    logger.debug("%s: %s", id.c_str(), cfgFile.c_str());

    // Read configuration
    JsonFileHandler jFile(cfgFile);
    if (!jFile.read()) {
	logger.fatal("Cannot open processor config. file '" + cfgFile + "'. Exiting.");
    }
    Config pcfg(jFile.getData());

    // Evaluate configuration entries 
    // 1. Input file(s), outputs and log)
    char curdir[PATH_MAX];
    getwd(curdir);
    if (chdir(taskFld) < 0) {
	logger.error("Cannot change to task folder " + taskFld);
	return false;
    }

    p_inputs = FileTools::filesInFolder(pcfg["input"].asString());
    if (p_inputs.size() < 1) {
        logger.error("No input files provided to the processor %s", proc.c_str());
        return false;
    }
    //logger.debug("Processing task %s will process %s", taskId.c_str(), p_input.c_str());

    p_outputs = FileTools::filesInFolder(pcfg["output"].asString());
    p_logs    = FileTools::filesInFolder(pcfg["log"].asString());

    chdir(curdir);

    // 2. Processor subfolder name (folder under QPF_WA/bin/"
    string p_processor = pcfg["processor"].asString();
    // 3. Processor entire subfolder name
    string p_proc_dir = qpfProcsPath;  // + "/" + p_processor
    // 4. Main script to invoke processor (something like driver.py)
    string p_script = pcfg["script"].asString();
    string p_args = pcfg["args"].asString();

    string p_output = str::join(p_outputs, ",");
    string p_log    = str::join(p_logs, ",");
    // Expand/process output and log (if needed)
    if (isSubstitutionRules(p_output)) { p_output = doRules(p_output); }
    if (isSubstitutionRules(p_log)) { p_log = doRules(p_log); }

    // Expand arguments
    // Valid placeholders are:
    // - {input}  : Content of the input variable, comma separated if multiple
    // - {output} : Content of the output variable, comma separated if multiple
    // - {log}    : Content of the log variable
    // p_args = re.sub('{input}',
    //                    ','.join(p_input),
    //                    re.sub('{output}',
    //                           ','.join(p_output),
    //                           re.sub('{log}',
    //                                  ','.join(p_log),
    //                                  p_args)))
    expansor = [](vector<string> x){ return str::join(x, ","); };
    
    Config entries(pcfg);
    entry_in = expansor(p_input)
    entry_out = expansor(p_output)
    entry_log = expansor(p_log)
    entries.update({"input": entry_in, "output": entry_out, "log": entry_log})
    for kitem, vitem in entries.items():
        p_args = re.sub('{' + kitem + '}', str(vitem), p_args)

    //logger.debug(I + 'Arguments: %s', p_args)

    // Prepare Docker folder mapping, just in case...
    taskFld_img = TaskAgent.QPFDckImageRunPath + "/" + taskId
    p_proc_dir_img = TaskAgent.QPFDckImageProcPath  // + "/" + processor

    p_maps = {"task_dir": [taskFld, taskFld_img],
                   "proc_dir": [p_proc_dir, p_proc_dir_img]}
    dck_mapping = {}
    for mkey, mval in p_maps.items():
        dck_mapping[mval[0]] = {"bind": mval[1], "mode": "rw"}

    dck_image = pcfg["image"]  // Processor.QPFDckImageDefault
    dck_workdir = taskFld_img


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
    static const clock_t tOffset = 5 * CLOCKS_PER_SEC; // 5 seconds

    vector<string> containersToRemoveNow;

    clock_t now = clock();
    for (auto & p: containersToRemove) {
	clock_t clk = p.first;
	if ((now - clk) > tOffset) { containersToRemoveNow.push_back(p.second); }	    
    }

    for (auto & c: containersToRemoveNow) {
	// remove container
	// dckMng.???
	logger.debug("Removing container %s", c.c_str());
    }

    containersToRemove.erase(containersToRemove.begin(),
			     containersToRemove.begin() + containersToRemoveNow.size());
}

//----------------------------------------------------------------------
// Method: atom_move
//----------------------------------------------------------------------
void TaskAgent::atom_move(string src, string dst)
{
}

//----------------------------------------------------------------------
// Method: prepareOutputs
// Prepare outputs, placing them in the outputs folder or in the remote
// outputs folder to be sent to the commander host
//----------------------------------------------------------------------
void TaskAgent::prepareOutputs()
{
    static FileNameSpec fns;

    vector<string> logFiles = FileTools::filesInFolder(taskFolder + "/log", "log");
    vector<string> outFiles = FileTools::filesInFolder(taskFolder + "/out");

    // Move the outputs to the outbox folder, so they are sent to the archive
    for (auto & f: logFiles) {
	ProductMeta meta;
	if (! fns.parse(f, meta)) {
	    logger.error("Cannot parse file name for product %s", f.c_str());
	    continue;
	}
	if (! ProductLocator::toLocalOutputs(meta, wa)) {
	    logger.error("Cannot move %s to local outputs folder", f.c_str());
	}
    }
    
    // Move, instead, the output files to the inbox, so they are checked
    // if they trigger a new rule
    for (auto & f: outFiles) {
	ProductMeta meta;
	if (! fns.parse(f, meta)) {
	    logger.error("Cannot parse file name for product %s", f.c_str());
	    continue;
	}
	if (! ProductLocator::toLocalInbox(meta, wa)) {
	    logger.error("Cannot move %s to local inbox folder", f.c_str());
	}
    }
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

