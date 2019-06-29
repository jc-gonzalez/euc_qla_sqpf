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

    json n = cfg["network"];
    
    thisNode = id;
    commander = n["commander"];
    thisIsCommander = (id == commander);

    json n_p = n["processingNodes"];
    for (auto const & x : n_p.items()) {
	json & xo = x.value(); 
	std::string const & name = x.key();
	nodeName.push_back(name);
	if (name != commander) { nodesButComm.push_back(name); }
	nodeAddress.push_back(xo["address"]);
	nodePort.push_back(xo["port"].get<int>());
	nodeServerUrl.push_back("http://" + xo["address"].get<std::string>() +
				":" + std::to_string(xo["port"].get<int>()));
	nodeNumOfAgents.push_back(xo["agents"].get<int>());
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

