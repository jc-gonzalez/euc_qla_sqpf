/******************************************************************************
 * File:    taskorc.h
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
 *   Declare TaskOrchestrator class
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

#ifndef TASKORCHESTRATOR_H
#define TASKORCHESTRATOR_H

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
#include "taskmng.h"
#include "types.h"

//==========================================================================
// Class: TaskOrchestrator
//==========================================================================
class TaskOrchestrator {

public:
    //----------------------------------------------------------------------
    // Method: TaskOrchestrator
    //----------------------------------------------------------------------
    TaskOrchestrator(Config & _cfg, string _id);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~TaskOrchestrator();

    //----------------------------------------------------------------------
    // Method: schedule
    //----------------------------------------------------------------------
    bool schedule(ProductMeta & meta, TaskManager & manager);

protected:

private:
    //----------------------------------------------------------------------
    // Method: checkRules
    //----------------------------------------------------------------------
    bool checkRules(ProductMeta & prod);

private:
    Config & cfg;
    string id;
    string workArea;
    map<string, map<string, string>> rules;
    map<string, string> processors;

    vector<map<string, string>> firedRules;

    Logger logger;
};

#endif // TASKORCHESTRATOR_H
