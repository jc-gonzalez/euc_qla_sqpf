/******************************************************************************
 * File:    procnet.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.ProcessingNetwork
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
 *   Declare ProcessingNetwork class
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

#ifndef PROCESSINGNETWORK_H
#define PROCESSINGNETWORK_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"

//==========================================================================
// Class: ProcessingNetwork
//==========================================================================
class ProcessingNetwork {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ProcessingNetwork(Config & _cfg, string & _name, int _bMode);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~ProcessingNetwork();

public:
    Config & cfg;
    string id;
    BalancingMode balanceMode;

    string thisNode;
    string commander;
    string commanderUrl;

    bool thisIsCommander;

    vector<string> nodeName;
    vector<string> nodesButComm;
    vector<string> nodeAddress;
    vector<string> nodeServerUrl;
    vector<int> nodePort;
    vector<int> nodeNumOfAgents;

    int numOfNodes;

    int commanderNum;
    int thisNodeNum;
    int thisNodeNumOfAgents;
    string thisNodeAddress;
    int thisNodePort;

    map<string, vector<string> > nodeAgents;
};

#endif // PROCESSINGNETWORK_H
