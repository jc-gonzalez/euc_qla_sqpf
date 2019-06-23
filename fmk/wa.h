/******************************************************************************
 * File:    wa.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.WorkArea
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
 *   Declare WorkArea class
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

#ifndef WORKAREA_H
#define WORKAREA_H

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
// Class: WorkArea
//==========================================================================
class WorkArea {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    WorkArea();

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    WorkArea(string _wa);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~WorkArea();

    //----------------------------------------------------------------------
    // Method: dump
    //----------------------------------------------------------------------
    void dump();

public:
    string wa;

    string procArea;

    string localInbox;
    string localOutputs;

    string archive;

    string reproc;

    string serverBase;

    string remoteOutputs;
    string remoteInbox;

    string run;
    string runTools;

    string sessionId;

    string sessionDir;

    string tasks;
    string logs;

};

#endif // WORKAREA_H
