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

#include "dbhdlpostgre.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
DataManager::DataManager(Config & _cfg, ProcessingNetwork & _net)
    : cfg(_cfg), net(_net),
      logger(Log::getLogger("datmng"))
{
    
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
DataManager::~DataManager()
{
}

//----------------------------------------------------------------------
// Method: initializeDB
// Initialize the DB
//----------------------------------------------------------------------
void DataManager::initializeDB()
{
    std::unique_ptr<DBHandler> dbHdl(new DBHdlPostgreSQL(net, logger));

    dbHdl->setConnectionParams(cfg["db"]["host"].asString(),
			       cfg["db"]["port"].asInt(),
			       cfg["db"]["name"].asString(),
			       cfg["db"]["user"].asString(),
			       cfg["db"]["pwd"].asString());
    
    // Check that connection with the DB is possible
    if (dbHdl->openConnection()) {
	logger.info("Connection params. initialized for DB %s",
		    cfg["db"]["name"].asString().c_str());
	dbHdl->closeConnection();
    }
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
void DataManager::storeTaskInfo(string & taskId, int taskStatus,
                                string & taskInfo, bool initial)
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
