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
#include "jsonfhdl.h"

#include <unistd.h>
#include <sys/types.h>

const string TaskAgent::QPFDckImageDefault("debian");
const string TaskAgent::QPFDckImageRunPath("/qpf/run");
const string TaskAgent::QPFDckImageProcPath("/qlabin");

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskAgent::TaskAgent(WorkArea _wa, string _ident,
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

    // Initialize user variables
    std::stringstream ss;
    ss << getuid();
    uid = ss.str();
    uname = string(getenv("USER"));
}

//----------------------------------------------------------------------
// Method: getFiles
// Get the files according to a item expresion (i.e.: in/*.fits)
//----------------------------------------------------------------------
vector<string> TaskAgent::getFiles(string item)
{
    return FileTools::filesInFolder(str::getDirName(item.c_str()),
				    str::getExtension(item.c_str()));
}

//----------------------------------------------------------------------
// Method: substitute
//----------------------------------------------------------------------
string TaskAgent::substitute(string value, string rule)
{
    size_t pos = rule.find("=>");
    string from = rule.substr(0, pos);
    string to = rule.substr(pos+2);
    return str::replaceAll(value, from, to);
}

//----------------------------------------------------------------------
// Method: getubstitutionRules
// Get the list of substitution rules
//----------------------------------------------------------------------
std::tuple< string, vector<string> > TaskAgent::getSubstitutionRules(string item)
{
    auto v = str::split(item.substr(1, item.length()-2), ':');
    return std::make_tuple(v.at(0), str::split(v.at(1), ','));
}

//----------------------------------------------------------------------
// Method: isSubstitutionRules
// Determine if a certain item has rules inside
//----------------------------------------------------------------------
bool TaskAgent::isSubstitutionRules(string item)
{
    logger.debug("Subs.rule: %s", item.c_str());
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
    string from_var;
    vector<string> rules;
    std::tie(from_var, rules) = getSubstitutionRules(item);

    logger.error("Rule: from '%s', do {%s}", from_var.c_str(),
                 str::join(rules, "} and {").c_str());
    string value;
    if (from_var == "input") {
	value = str::join(p_inputs, " ");
    } else if (from_var == "output") {
	value = str::join(p_outputs, " ");
    } else if (from_var == "log") {
	value = str::join(p_logs, " ");
    } else {
	value = pcfg[from_var].asString();
    }

    for (auto & rule: rules) {
	value = substitute(value, rule);
    }

    logger.error("Final result: '%s'", value.c_str());
    return str::split(value, ' ');
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
    msgSpec.pop_back();
    oq->push(string(id));
    oq->push(std::move(msgSpec));
}

//----------------------------------------------------------------------
// Method: prepareNewTask
// Prepare environment to launch new container for new task
//----------------------------------------------------------------------
bool TaskAgent::prepareNewTask(string taskId, string taskFld, string proc)
{
    string p_proc_dir = wa.procArea;  // + "/" + p_processor
    // Read processor config. file
    string cfgFile(taskFld + "/" + proc + ".cfg");
    logger.debug("%s: %s", id.c_str(), cfgFile.c_str());

    // Read configuration
    JsonFileHandler jFile(cfgFile);
    if (!jFile.read()) {
	logger.fatal("Cannot open processor config. file '" + cfgFile + "'. Exiting.");
    }
    pcfg = jFile.getData();

    // Evaluate configuration entries
    // 1. Input file(s), outputs and log)
    char curdir[200];
    getcwd(curdir, 200);
    if (chdir(taskFld.c_str()) < 0) {
	logger.error("Cannot change to task folder " + taskFld);
	return false;
    }
    logger.debug("Moving from %s to %s", curdir, taskFld.c_str());

    p_inputs = getFiles(pcfg["input"].asString());
    if (p_inputs.size() < 1) {
        logger.error("No input files provided to the processor %s", proc.c_str());
        return false;
    }
    string p_input = str::join(p_inputs, ",");
    logger.debug("Processing task %s will process %s", taskId.c_str(), p_input.c_str());

    string p_output = pcfg["output"].asString();
    if (isSubstitutionRules(p_output)) {
	p_outputs = doRules(p_output);
    } else {
	p_outputs = getFiles(p_output);
    }
    p_output = str::join(p_outputs, ",");
    logger.debug("Output: %s", p_output.c_str());

    string p_log    = pcfg["log"].asString();
    if (isSubstitutionRules(p_log)) {
	p_logs = doRules(p_log);
    } else {
	p_logs = getFiles(p_log);
    }
    p_log = str::join(p_logs, ",");
    logger.debug("Log: %s", p_log.c_str());

    chdir(curdir);
    logger.debug("Back in %s", curdir);
    
    // 2. Processor subfolder name (folder under QPF_WA/bin/"
    string p_processor = pcfg["processor"].asString();
    // 3. Processor entire subfolder name
    //string p_proc_dir = wa.procArea;  // + "/" + p_processor
    // 4. Main script to invoke processor (something like driver.py)
    string p_script = pcfg["script"].asString();

    // 5. Arguments
    string p_args = pcfg["args"].asString();

    pcfg["input"]  = p_input;
    pcfg["output"] = p_output;
    pcfg["log"]    = p_log;
    logger.debug(">>> " + pcfg.str());

    for (auto & kv: pcfg) {
	p_args = str::replaceAll(p_args, "{" + kv.first + "}", kv.second.asString());
    }

    logger.debug("Arguments: %s", p_args.c_str());

    // Prepare Docker folder mapping, just in case...
    string taskFld_img = TaskAgent::QPFDckImageRunPath + "/" + taskId;
    string p_proc_dir_img = TaskAgent::QPFDckImageProcPath;  // + "/" + processor

    // Prepare Docker launch variables
    dck_image   = pcfg["image"].asString();  // Processor.QPFDckImageDefault
    dck_exe     = pcfg["exe"].asString(); 
    p_args.insert(0, (p_proc_dir_img + "/" + p_processor + "/" +
		      pcfg["script"].asString() + " "));
    dck_args    = str::split(p_args, ' ');
    dck_workdir = taskFld_img;
    dck_mapping = { {taskFld, taskFld_img + ":rw"},
                    {p_proc_dir, p_proc_dir_img} };

    return true;
}

//----------------------------------------------------------------------
// Method: launchContainer
// Launches container on the given task folder
//----------------------------------------------------------------------
bool TaskAgent::launchContainer(string & contId)
{
    vector<string> opts {"--workdir", dck_workdir,
	    "--env", "UID=" + uid,
	    "--env", "UNAME=" + uname,
	    "--env", "WDIR=" + dck_workdir};

    string cmd_line;
    
    if (! dckMng->createContainer(dck_image, opts, dck_mapping,
				  dck_exe, dck_args,
                                  containerId, cmd_line)) {
        containerId = "";
	logger.error("Cannot launch container as follows: ");
	logger.fatal(cmd_line);
	return false;
    }

    logger.debug("CMD: " + cmd_line);

    contId = containerId;
    return true;
}

//----------------------------------------------------------------------
// Method: inspectContainer
//----------------------------------------------------------------------
string TaskAgent::inspectContainer(string cntId, bool fullInfo, string filter)
{
    // Get inspect information
    std::stringstream info;
    info.str(fullInfo ? "" : "'" + filter + "'");
    if (!dckMng->getInfo(cntId, info)) {
	logger.error("Cannot inspect container with id %s", cntId.c_str());
    }
    /*
    bool ok = true;
    int iters = 0;
    do {
	info.str(fullInfo ? "" : "'" + filter + "'");
	ok = dckMng->getInfo(cntId, info);
	if (!ok) { delay(10); ++iters; }
	if (iters > 10) {
	    logger.error("Cannot inspect container with id %s", cntId.c_str());
	    return std::string("");
	}
    } while (! ok);
    */
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
	//inspect = inspectContainer(contId);
	//string statusLowStr(TaskStatusStr[TASK_RUNNING]);
	//str::toLower(statusLowStr);
	for (auto & s : vector<string> {"true", taskId, contId,
                                            inspect, "1", "scheduled"}) {
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
	if (dckMng->remove(c)) {
	    logger.debug("Removing container " + c);
	} else {
	    logger.warn("Couldn't remove container " + c);
	}
    }

    containersToRemove.erase(containersToRemove.begin(),
			     containersToRemove.begin() + containersToRemoveNow.size());
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

    logger.debug("logs: " + str::join(logFiles, ","));
    logger.debug("outputs: " + str::join(outFiles, ","));

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
	    logger.info("New task launched in container: " + contId);
	    containerId = contId;
	    containerSpectrum.append(contId, "scheduled");
	    delay(200);
	}
    } else {
	contId = containerId;
    }

    inspect = inspectContainer(contId, false, inspectSelection);
    if (! inspect.empty()) {
	logger.debug("INSPECT>> " + inspect);
	jso jinspect;
	parser.parse(inspect, jinspect);

	string inspStatus = jinspect["State"]["Status"].asString();
	int inspCode      = jinspect["State"]["ExitCode"].asInt();
	status = stateToTaskStatus(inspStatus, inspCode);
	string statusLowStr(TaskStatusStr[status]);
	str::toLower(statusLowStr);
	logger.debug("         Current status: %s (%s, %d)", 
		     statusLowStr.c_str(), inspStatus.c_str(), inspCode); 
	for (auto & s : vector<string> {"false", taskId, contId,
                                            inspect, "1", statusLowStr}) {
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

	    logger.debug("New task id queued at Task Agent %s: %s",
			 id.c_str(), itaskId.c_str());
	    logger.debug("Execution to be done in work. dir. %s",
			 itaskFolder.c_str());
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

