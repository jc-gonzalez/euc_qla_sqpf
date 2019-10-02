/******************************************************************************
 * File:    master.h
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
 *   Declare Master class
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

#ifndef MASTER_H
#define MASTER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

#include <random>
#include "limits.h"

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"
#include "wa.h"
#include "procnet.h"
#include "taskorc.h"
#include "taskmng.h"
#include "datamng.h"
#include "q.h"

#include "log.h"

#include "masterserver.h"
#include "masterrequester.h"

//==========================================================================
// Class: Master
//==========================================================================
class Master {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    Master(string _cfg, string _id, int _port, string _wa, int _bMode);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~Master();

    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    void run();

    //----------------------------------------------------------------------
    // Method: getHostInfo
    //----------------------------------------------------------------------
    string getHostInfo();

protected:

private:
    //----------------------------------------------------------------------
    // Method: startSession
    //----------------------------------------------------------------------
    void startSession();

    //----------------------------------------------------------------------
    // Method: loadStateVector
    //----------------------------------------------------------------------
    void loadStateVector();

    //----------------------------------------------------------------------
    // Method: lookForSuspendedTasks
    //----------------------------------------------------------------------
    vector<string> & lookForSuspendedTasks();

    //----------------------------------------------------------------------
    // Method: appendProdsToQueue
    //----------------------------------------------------------------------
    void appendProdsToQueue(vector<string> & prods);

    //----------------------------------------------------------------------
    // Method: appendProdsToQueue
    //----------------------------------------------------------------------
    void appendProdsToQueue(Queue<string> & prods);

    //----------------------------------------------------------------------
    // Method: setDirectoryWatchers
    //----------------------------------------------------------------------
    void setDirectoryWatchers();

    //----------------------------------------------------------------------
    // Method: getNewEntries
    //----------------------------------------------------------------------
    bool getNewEntries();

    //----------------------------------------------------------------------
    // Method: getNewEntriesFromDirWatcher
    //----------------------------------------------------------------------
    bool getNewEntriesFromDirWatcher(DirWatcher * dw, Queue<string> & q);
 
    //----------------------------------------------------------------------
    // Method: checkIfProduct
    //----------------------------------------------------------------------
    bool checkIfProduct(string & fileName, ProductMeta & meta);

    //----------------------------------------------------------------------
    // Method: distributeProducts
    //----------------------------------------------------------------------
    void distributeProducts();

    //----------------------------------------------------------------------
    // Method: scheduleProductsForProcessing
    //----------------------------------------------------------------------
    void scheduleProductsForProcessing();

    //----------------------------------------------------------------------
    // Method: archiveOutputs
    //----------------------------------------------------------------------
    void archiveOutputs();

    //----------------------------------------------------------------------
    // Method: transferRemoteLocalArchiveToCommander
    //----------------------------------------------------------------------
    void transferRemoteLocalArchiveToCommander();

    //----------------------------------------------------------------------
    // Method: transferOutputsToCommander
    //----------------------------------------------------------------------
    void transferOutputsToCommander();

    //----------------------------------------------------------------------
    // Method: transferFilesToCommander
    // Transfer (POST) outputs to server/XXX end of commander server
    //----------------------------------------------------------------------
    void transferFilesToCommander(Queue<string> & prodQueue,
                                  string route = string("/outputs"));
    
    //----------------------------------------------------------------------
    // Method: gatherNodesStatus
    //----------------------------------------------------------------------
    void gatherNodesStatus();

    //----------------------------------------------------------------------
    // Method: gatherTasksStatus
    // Collects all nodes tasks info
    //----------------------------------------------------------------------
    void gatherTasksStatus();
    
    //----------------------------------------------------------------------
    // Method: runMainLoop
    //----------------------------------------------------------------------
    void runMainLoop();

    //----------------------------------------------------------------------
    // Method: delay
    // Waits for a small time lapse for system sync
    //----------------------------------------------------------------------
    void delay(int ms);
    
    //----------------------------------------------------------------------
    // Method: terminate
    //----------------------------------------------------------------------
    void terminate();

    //----------------------------------------------------------------------
    // Method: genRandomNode
    //----------------------------------------------------------------------
    int genRandomNode();

private:
    string cfgFileName;
    string id;
    string workArea;
    int port;
    int balanceMode;

    WorkArea wa;
    ProcessingNetwork * net;

    Config cfg;

    int masterLoopSleep_ms; //ms

    TaskOrchestrator * tskOrc;
    TaskManager * tskMng;
    DataManager * dataMng;

    MasterServer * httpServer;
    MasterRequester * httpRqstr;

    Queue<string> inboxProdQueue;
    Queue<string> reprocProdQueue;
    vector<DirWatchedAndQueue> dirWatchers;

    vector<string> productsFromSuspTasks;
    Queue<string> productList;

    Queue<string> productsForProcessing;
    Queue<string> productsForArchival;

    Queue<string> outputProducts;

    bool nodeInfoIsAvailable;
    json nodeInfo;

    typedef int(*SelectNodeFn)(Master*);
    SelectNodeFn selectNodeFn;

    vector<bool> nodeStatusIsAvailable;
    vector<json> nodeStatus;

    Logger logger;
    
public:
    int lastNodeUsed;
    vector<double> loads;

private:
    std::uniform_int_distribution<int> * dist;
    static std::mt19937 mt;
};

#endif // MASTER_H
