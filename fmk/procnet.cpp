/******************************************************************************
 * File:    procnet.cpp
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
 *   Implement ProcessingNetwork class
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

#include "procnet.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ProcessingNetwork::ProcessingNetwork(Config & _cfg, string & _name, int _bMode)
    : cfg(_cfg), id(_name), balanceMode(BalancingModeEnum(_bMode))
{
    string tplTaskAgId = "TskAgent_{:02d}_{:02d}";

    js n = cfg["network"];

    //std::cerr << n.asObject().str() << '\n';
    
    thisNode = id;
    commander = n["commander"].asString();
    thisIsCommander = (id == commander);

    //std::cerr << "thisNode: " << thisNode << '\n';
    //std::cerr << "commmander: " << commander << '\n';

    js n_p = n["processingNodes"];
    for (auto x : n_p.asObject()) {
	js xo = x.second; 
	std::string name = x.first;
	nodeName.push_back(name);
	if (name != commander) { nodesButComm.push_back(name); }
	nodeAddress.push_back(xo["address"].asString());
	nodePort.push_back(xo["port"].asInt());
	nodeServerUrl.push_back("http://" + xo["address"].asString() +
				":" + xo["port"].asString());
	nodeNumOfAgents.push_back(xo["agents"].asInt());
    }
    numOfNodes = nodeName.size();

    commanderNum = indexOf<std::string>(nodeName, commander);
    thisNodeNum = indexOf<std::string>(nodeName, id);
    thisNodeNumOfAgents = nodeNumOfAgents[thisNodeNum];
    thisNodeAddress = nodeAddress[thisNodeNum];
    thisNodePort = nodePort[thisNodeNum];

    char buff[40];
    for (int k = 0; k < numOfNodes; ++k) {
	vector<string> arr;
	for (int i = 0; i < nodeNumOfAgents.at(k); ++i) {
	    sprintf(buff, "TskAgent_%02d_%02d", k + 1, i + 1);
	    arr.push_back(std::string(buff));
	}
	nodeAgents[nodeName.at(k)] = arr;
    }
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
ProcessingNetwork::~ProcessingNetwork()
{
}

