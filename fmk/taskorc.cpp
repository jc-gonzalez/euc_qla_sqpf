/******************************************************************************
 * File:    taskorc.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.TaskOrchestrator
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
 *   Implement TaskOrchestrator class
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

#include "taskorc.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskOrchestrator::TaskOrchestrator(Config & _cfg, string _id)
    : cfg(_cfg), id(_id),
      logger(Log::getLogger("tskorc"))
{
    workArea = cfg["general"]["workArea"].asString();
    js jrules = cfg["orchestration"]["rules"];
    for (auto & r: jrules.asArray()) {
        string rname(r["name"].asString());
	rules[rname] = map<string, string>({{"inputs", r["inputs"].asString()},
		    {"processing", r["processing"].asString()}});
    }

    js jprocs =  cfg["orchestration"]["processing"];
    for (auto & kv: jprocs.asObject()) {
	processors[kv.first] = kv.second.asString();
    }
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
TaskOrchestrator::~TaskOrchestrator()
{
}

//----------------------------------------------------------------------
// Method: checkRules
//----------------------------------------------------------------------
bool TaskOrchestrator::checkRules(ProductMeta & prod)
{
    firedRules.clear();
    
    string pType = prod["type"].asString();
    
    for (auto & kv: rules) {
	auto rname = kv.first;
	auto r = kv.second;

	string inputs = r["inputs"] + ",";
	if (inputs.find(pType) == string::npos) {
	    logger.error("Product type %s does not appear in the configuration", pType);
	    continue;
	}
	
	string procId = r["processing"];
	if (processors.find(procId) == processors.end()) {
	    logger.error("Cannot process %s (rule fired is %s): no such processor",
			 rname, procId);
	    continue;
	}
	
	string processor = processors[procId];
	firedRules.push_back(map<string, string>({{"name", rname},
			{"processor", processor}}));
	logger.info("Rule %s fired by %s product", rname, pType);
    }

    return !firedRules.empty();
}

//----------------------------------------------------------------------
// Method: schedule
//----------------------------------------------------------------------
bool TaskOrchestrator::schedule(ProductMeta & meta, TaskManager & manager)
{
    if (!checkRules(meta)) {
	logger.warn("No rule found for %s product %s",
		    meta["type"].asString(), meta["fileinfo"]["bname"].asString());
	return false;
    }

    for (auto & v: firedRules) {
	manager.schedule(meta, v["processor"]);
    }
}
