/******************************************************************************
 * File:    taskagent.h
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
 *   Declare TaskAgent class
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

#ifndef TASKAGENT_H
#define TASKAGENT_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <ctime>
#include <chrono>
using namespace std::chrono;

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------
#include "log.h"

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"
#include "wa.h"
#include "q.h"
#include "cs.h"

#include "cntrmng.h"

//==========================================================================
// Class: TaskAgent
//==========================================================================
class TaskAgent {

    typedef high_resolution_clock::time_point   hires_time;
    
public:
    //----------------------------------------------------------------------
    // Method: TaskAgent
    //----------------------------------------------------------------------
    TaskAgent(WorkArea _wa, string _ident,
              Queue<string> * _iq, Queue<string> * _oq, Queue<string> * _tq,
              bool _isCommander);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~TaskAgent();

    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    void run();

protected:

private:
    //----------------------------------------------------------------------
    // Method: init
    //----------------------------------------------------------------------
    virtual void init();

    //----------------------------------------------------------------------
    // Method: getFiles
    // Get the files according to a item expresion (i.e.: in/*.fits)
    //----------------------------------------------------------------------
    vector<string> getFiles(string item);
    
    //----------------------------------------------------------------------
    // Method: substitute
    //----------------------------------------------------------------------
    string substitute(string value, string rule);

    //----------------------------------------------------------------------
    // Method: get_substitution_rules
    //----------------------------------------------------------------------
    std::tuple< string, vector<string> > getSubstitutionRules(string item);

    //----------------------------------------------------------------------
    // Method: isSubstitutionRules
    //----------------------------------------------------------------------
    bool isSubstitutionRules(string item);

    //----------------------------------------------------------------------
    // Method: stateToTaskStatus
    //----------------------------------------------------------------------
    TaskStatus stateToTaskStatus(string inspStatus, int inspCode);

    //----------------------------------------------------------------------
    // Method: isEnded
    //----------------------------------------------------------------------
    bool isEnded(TaskStatus st);

    //----------------------------------------------------------------------
    // Method: doRules
    //----------------------------------------------------------------------
    vector<string> doRules(string item);

    //----------------------------------------------------------------------
    // Method: sendSpectrumToMng
    //----------------------------------------------------------------------
    void sendSpectrumToMng();

    //----------------------------------------------------------------------
    // Method: prepareNewTask
    //----------------------------------------------------------------------
    bool prepareNewTask(string taskId, string taskFld, string proc);

    //----------------------------------------------------------------------
    // Method: launchContainer
    //----------------------------------------------------------------------
    bool launchContainer(string & contId);

    //----------------------------------------------------------------------
    // Method: inspectContainer
    //----------------------------------------------------------------------
    string inspectContainer(string cntId, bool fullInfo = true,
                            string filter = string(""));
    
    //----------------------------------------------------------------------
    // Method: launchNewTask
    //----------------------------------------------------------------------
    std::string launchNewTask();

    //----------------------------------------------------------------------
    // Method: scheduleContainerForRemoval
    //----------------------------------------------------------------------
    void scheduleContainerForRemoval();

    //----------------------------------------------------------------------
    // Method: removeOldContainers
    //----------------------------------------------------------------------
    void removeOldContainers();

    //----------------------------------------------------------------------
    // Method: prepareOutputs
    //----------------------------------------------------------------------
    void prepareOutputs();

    //----------------------------------------------------------------------
    // Method: monitorTasks
    //----------------------------------------------------------------------
    void monitorTasks();

    //----------------------------------------------------------------------
    // Method: delay
    // Waits for a small time lapse for system sync
    //----------------------------------------------------------------------
    void delay(int ms);
    
    //----------------------------------------------------------------------
    // Method: timeNow
    // Returns a high resolution clock time stamp
    //----------------------------------------------------------------------
    hires_time timeNow();
    
private:
    string id;
    WorkArea wa;
    Queue<string> * iq;
    Queue<string> * oq;
    Queue<string> * tq;
    bool isCommander;

    bool iAmQuitting;
    
    Queue<string> taskQueue;

    string taskId;
    string taskFolder;
    string processor;
    string containerId;
    ContainerSpectrum containerSpectrum;
    string inspect;
    TaskStatus status;

    string uid;
    string uname;
    
    vector< pair<hires_time, string> > containersToRemove;
    
    std::shared_ptr<ContainerMng> dckMng;

    Config pcfg;
    vector<string> p_inputs;
    vector<string> p_outputs;
    vector<string> p_logs;

    string dck_image;
    string dck_exe;
    vector<string> dck_args;
    string dck_workdir;
    map<string, string> dck_mapping;
    
    Logger logger;

    static const string QPFDckImageDefault;
    static const string QPFDckImageRunPath;
    static const string QPFDckImageProcPath;

    static const string inspectSelection;
};

#endif // TASKAGENT_H
