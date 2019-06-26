/******************************************************************************
 * File:    masterserver.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.MasterServer
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
 *   Declare MasterServer class
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

#ifndef MASTERSERVER_H
#define MASTERSERVER_H

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
#include "httpcommsrv.h"

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "wa.h"

class Master;

//==========================================================================
// Class: MasterServer
//==========================================================================
class MasterServer : public HttpCommServer {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    MasterServer(Master * hdl, int prt, WorkArea & _wa);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~MasterServer();

    //----------------------------------------------------------------------
    // Method: launch
    //----------------------------------------------------------------------
    void launch();
    
protected:

private:
    //----------------------------------------------------------------------
    // Method: run
    //----------------------------------------------------------------------
    void run();

private:
    Master * mhdl;
    WorkArea & wa;
};

#endif // MASTERSERVER_H
