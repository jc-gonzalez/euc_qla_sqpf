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
#include "json.h"

#include "cntrmng.h"

//==========================================================================
// Class: TaskAgent
//==========================================================================
class TaskAgent {

public:
    //----------------------------------------------------------------------
    // Method: TaskAgent
    //----------------------------------------------------------------------
    TaskAgent(WorkArea & _wa, string _ident,
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
    // Method: substitute
    //----------------------------------------------------------------------
    void substitute(dict & rule);

    //----------------------------------------------------------------------
    // Method: get_substitution_rules
    //----------------------------------------------------------------------
    void get_substitution_rules();

    //----------------------------------------------------------------------
    // Method: is_substitution_rules
    //----------------------------------------------------------------------
    void is_substitution_rules();

    //----------------------------------------------------------------------
    // Method: stateToTaskStatus
    //----------------------------------------------------------------------
    TaskStatus stateToTaskStatus(string inspStatus, int inspCode);

    //----------------------------------------------------------------------
    // Method: isEnded
    //----------------------------------------------------------------------
    bool isEnded(TaskStatus st);

    //----------------------------------------------------------------------
    // Method: do_rules
    //----------------------------------------------------------------------
    void do_rules(dict item);

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
    // Method: atom_move
    //----------------------------------------------------------------------
    void atom_move(string src, string dst);

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
    
private:
    string id;
    WorkArea & wa;
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

    vector< pair<clock_t, string> > containersToRemove;
    
    std::shared_ptr<ContainerMng> dckMng;
    
    Logger logger;
};

#endif // TASKAGENT_H
