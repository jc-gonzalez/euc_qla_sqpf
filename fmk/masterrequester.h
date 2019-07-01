/******************************************************************************
 * File:    masterrequester.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.MasterRequester
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
 *   Declare MasterRequester class
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

#ifndef MASTERREQUESTER_H
#define MASTERREQUESTER_H

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

class RWC;

//==========================================================================
// Class: MasterRequester
//==========================================================================
class MasterRequester {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    MasterRequester();

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~MasterRequester();

    //----------------------------------------------------------------------
    // Method: setServerUrl
    // Sets the server URL
    //----------------------------------------------------------------------
    void setServerUrl(string srvUrl);

    //----------------------------------------------------------------------
    // Method: requestData
    // Sets the server URL
    //----------------------------------------------------------------------
    bool requestData(string srvUrl, string & resp,
                     string contentType = string("application/octet-stream"));

    //----------------------------------------------------------------------
    // Method: postFile
    // Sets the server URL
    //----------------------------------------------------------------------
    bool postFile(string route, string fileName,
                  string contentType = string("application/octet-stream"));

private:
    string serverUrl;
    RWC * rwcHdl;
};

#endif // MASTERREQUESTER_H
