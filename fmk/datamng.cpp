/******************************************************************************
 * File:    datamng.cpp
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
 *   Implement DataManager class
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

#include "datamng.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
DataManager::DataManager(Config & _cfg) : cfg(_cfg)
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
DataManager::~DataManager()
{
}

//----------------------------------------------------------------------
// Method: connect
//----------------------------------------------------------------------
void DataManager::connect()
{
    
}

//----------------------------------------------------------------------
// Method: storeProductInfo
//----------------------------------------------------------------------
void DataManager::storeProductInfo(ProductMeta & m)
{

}

//----------------------------------------------------------------------
// Method: storeTaskInfo
//----------------------------------------------------------------------
void DataManager::storeTaskInfo(string taskId, int taskStatus,
                                dict & taskInfo, bool initial)
{

}

//----------------------------------------------------------------------
// Method: storeProductQueue
//----------------------------------------------------------------------
void DataManager::storeProductQueue(queue<ProductName> & q)
{

}

//----------------------------------------------------------------------
// Method: storeHostsSpectra
//----------------------------------------------------------------------
void DataManager::storeHostsSpectra(dict & info)
{
    
}
