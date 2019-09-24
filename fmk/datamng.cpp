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
    dbHdl = std::unique_ptr<DBHandler>(new DBHdlPostgreSQL(net, logger));

    dbHdl->setConnectionParams(cfg["db"]["host"].get<std::string>(),
                               std::stod(cfg["db"]["port"].get<std::string>()),
                               cfg["db"]["name"].get<std::string>(),
                               cfg["db"]["user"].get<std::string>(),
                               cfg["db"]["pwd"].get<std::string>());
    
    // Check that connection with the DB is possible
    if (dbHdl->openConnection()) {
        logger.info("Connection params. initialized for DB " +
                    cfg["db"]["name"].get<std::string>());
        dbHdl->closeConnection();
    }
}

//----------------------------------------------------------------------
// Method: storeProducts
//----------------------------------------------------------------------
void DataManager::storeProducts(ProductMetaList & vm)
{
    try {
        // Check that connection with the DB is possible
        if (!dbHdl->openConnection()) {
            logger.warn("Cannot establish connection to database");
        }
        dbHdl->storeProducts(vm);
    } catch (RuntimeException & e) {
        logger.fatal(e.what());
    }

    // Close connection
    dbHdl->closeConnection();
}

//----------------------------------------------------------------------
// Method: storeTaskInfo
//----------------------------------------------------------------------
void DataManager::storeTaskInfo(string & taskId, int taskStatus,
                                string & taskInfo, bool initial)
{
    try {
        // Check that connection with the DB is possible
        if (!dbHdl->openConnection()) {
            logger.warn("Cannot establish connection to database");
        }

        //logger.debug(taskInfo);
        json taskData = json::parse(taskInfo);
        json task = 
            {{"taskName", taskId},
                {"taskStatus", taskStatus},
                {"taskExitCode", taskData["State"]["ExitCode"].get<int>()},
                {"taskPath", taskData["Config"]["WorkingDir"].get<string>()},
                {"taskStart", taskData["State"]["StartedAt"].get<string>()},
                {"taskEnd", taskData["State"]["FinishedAt"].get<string>()},
                {"taskProgress", 1},
                {"taskInfo", taskInfo},
                {"taskData", taskData}};

        // Try to store the task data into the DB
        if (initial) { dbHdl->storeTask(task); }
        else         { dbHdl->updateTask(task); }

    } catch (RuntimeException & e) {
        logger.fatal(e.what());
    }

    // Close connection
    dbHdl->closeConnection();

    // If task is not finished, we are done
    if ((taskStatus != TASK_FINISHED) &&
        (taskStatus != TASK_FAILED)) { return; }

    // Otherwise, task is finished, save outputs metadata

    logger.debug(taskInfo);
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
