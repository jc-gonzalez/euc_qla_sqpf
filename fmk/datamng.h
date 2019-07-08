/******************************************************************************
 * File:    datamng.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.DataManager
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
 *   Declare DataManager class
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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

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
#include "log.h"
#include "procnet.h"

//==========================================================================
// Class: DataManager
//==========================================================================
class DataManager {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    DataManager(Config & _cfg, ProcessingNetwork & _net);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~DataManager();

    //----------------------------------------------------------------------
    // Method: initializeDB
    // Initialize the DB
    //----------------------------------------------------------------------
    void initializeDB();

    //----------------------------------------------------------------------
    // Method: storeProductInfo
    //----------------------------------------------------------------------
    void storeProductInfo(ProductMeta & m);

    //----------------------------------------------------------------------
    // Method: storeTaskInfo
    //----------------------------------------------------------------------
    void storeTaskInfo(string & taskId, int taskStatus,
                       string & taskInfo, bool initial);

    //----------------------------------------------------------------------
    // Method: storeProductQueue
    //----------------------------------------------------------------------
    void storeProductQueue(queue<ProductName> & q);

    //----------------------------------------------------------------------
    // Method: storeHostsSpectra
    //----------------------------------------------------------------------
    void storeHostsSpectra(dict & info);

private:
    ProcessingNetwork & net;
    Config & cfg;

    Logger logger;
};

#endif // DATAMANAGER_H
