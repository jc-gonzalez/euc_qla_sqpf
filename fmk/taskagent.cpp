/******************************************************************************
 * File:    taskagent.cpp
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
 *   Implement TaskAgent class
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

#include "taskagent.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
TaskAgent::TaskAgent(WorkArea & _wa, string _ident,
                     Queue<string> * _iq, Queue<string> * _oq, Queue<string> * _tq,
                     bool _isCommander)
    : wa(_wa), id(_ident), iq(_iq), oq(_oq), tq(_tq),
      isCommander(_isCommander)
{
    init();
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
TaskAgent::~TaskAgent()
{
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
void TaskAgent::init()
{
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
void TaskAgent::run()
{
    for(;;) {}
}

