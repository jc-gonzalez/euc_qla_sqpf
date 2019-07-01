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
    workArea = cfg["general"]["workArea"];
    json & jrules = cfg["orchestration"]["rules"];
    for (auto & r: jrules) {
        string rname(r["name"]);
        rules[rname] = map<string, string>({{"inputs", r["inputs"]},
                                            {"processing", r["processing"]}});
    }

    json & jprocs =  cfg["orchestration"]["processors"];
    for (auto & kv: jprocs.items()) {
        string const & k = kv.key();
        string const & v = kv.value();
        processors[k] = v;
        logger.debug("Storing proc.: %s => %s", k.c_str(), v.c_str());
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

    string const & pType = prod["type"];

    for (auto & kv: rules) {
        auto rname = kv.first;
        auto r = kv.second;

        string inputs = r["inputs"] + ",";
        if (inputs.find(pType) == string::npos) { continue; }

        string procId = r["processing"];
        if (processors.find(procId) == processors.end()) {
            logger.error("Cannot find %s processor config. "
                         "(rule fired is %s)",
                         procId.c_str(), rname.c_str());
            continue;
        }

        string processor = processors[procId];
        firedRules.push_back(map<string, string>({{"name", rname},
                        {"processor", processor}}));
        logger.info("Rule %s fired by %s product",
                    rname.c_str(), pType.c_str());
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
                    meta["type"].get<std::string>().c_str(),
                    meta["fileinfo"]["base"].get<std::string>().c_str());
        return false;
    }

    for (auto & v: firedRules) {
        manager.schedule(meta, v["processor"]);
    }
}
