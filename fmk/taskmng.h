/******************************************************************************
 * File:    taskmng.h
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
 *   Declare TaskManager class
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

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>
#include <tuple>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "datamng.h"
#include "types.h"
#include "wa.h"
#include "procnet.h"
#include "log.h"
#include "q.h"

class TaskAgent;

//==========================================================================
// Class: TaskManager
//==========================================================================
class TaskManager {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    TaskManager(Config & _cfg, string _id, 
                WorkArea & _wa, ProcessingNetwork & _net);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~TaskManager();

    //----------------------------------------------------------------------
    // Method: createAgents
    //----------------------------------------------------------------------
    void createAgents();

    //----------------------------------------------------------------------
    // Method: retrieveAgentsInfo
    //----------------------------------------------------------------------
    bool retrieveAgentsInfo(js & hi);

    //----------------------------------------------------------------------
    // Method: showSpectra
    //----------------------------------------------------------------------
    void showSpectra();

    //----------------------------------------------------------------------
    // Method: retrieveOutputs
    //----------------------------------------------------------------------
    void retrieveOutputs(Queue<string> & outputs);

    //----------------------------------------------------------------------
    // Method: updateTasksInfo
    //----------------------------------------------------------------------
    void updateTasksInfo(DataManager & datmng);

    //----------------------------------------------------------------------
    // Method: schedule
    //----------------------------------------------------------------------
    void schedule(ProductMeta & meta, string & processor);

protected:

private:
    //----------------------------------------------------------------------
    // Method: setDirectoryWatchers
    //----------------------------------------------------------------------
    void setDirectoryWatchers();

    //----------------------------------------------------------------------
    // Method: createAgents
    //----------------------------------------------------------------------
    void createAgent(string id, WorkArea wa,
		     Queue<string> * iq, Queue<string> * oq, Queue<string> * tq,
		     bool isComm);
    
    //----------------------------------------------------------------------
    // Method: createTaskId
    //----------------------------------------------------------------------
    string createTaskId(string tskAgId, int n);

    //----------------------------------------------------------------------
    // Method: createTaskFolders
    //----------------------------------------------------------------------
    void createTaskFolders(string & tskWkDir);

    //----------------------------------------------------------------------
    // Method: createTask
    //----------------------------------------------------------------------
    std::tuple<string, string> createTask(ProductMeta & meta, string & tskAgId,
					  int n, string & processor);

    //----------------------------------------------------------------------
    // Method: selectAgent
    //----------------------------------------------------------------------
    std::tuple<int, int> selectAgent();

    //----------------------------------------------------------------------
    // Method: updateAgent
    //----------------------------------------------------------------------
    void updateAgent(string & taskId, int agNum, string & agName, int agNumTsk);

    //----------------------------------------------------------------------
    // Method: updateContainer
    //----------------------------------------------------------------------
    void updateContainer(string & agName, string contId = string(""),
			 TaskStatus contStatus = TaskStatus(TASK_SCHEDULED));

    //----------------------------------------------------------------------
    // Method: terminate
    //----------------------------------------------------------------------
    void terminate();

private:
    Config & cfg;
    string id;
    WorkArea & wa;
    ProcessingNetwork & net;

    int thisNodeNum;
    int numOfAgents;
    double agentsHeartBeat;

    Queue<string> outboxProdQueue;

    vector<TaskAgent*> agents;
    vector<std::thread> agentThreads;

    vector<Queue<string>*> agentsInQueue;
    vector<Queue<string>*> agentsOutQueue;
    vector<Queue<string>*> agentsTskQueue;

    jso agentsInfo;

    string defaultProcCfg;
    
    Logger logger;
};

#endif // TASKMANAGER_H
