/******************************************************************************
 * File:    log.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.Log
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
 *   Implement Log class
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

#include "log.h"

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
Log::~Log()
{
}

//----------------------------------------------------------------------
// Method: initialize
//----------------------------------------------------------------------
void Log::initialize(std::string propertiesFileName)
{
    log4cpp::PropertyConfigurator::configure(propertiesFileName);
}

//----------------------------------------------------------------------
// Method: getRootLogger
//----------------------------------------------------------------------
Logger Log::getRootLogger()
{
    return log4cpp::Category::getRoot();
}
   
//----------------------------------------------------------------------
// Method: getLogger
//----------------------------------------------------------------------
Logger Log::getLogger(std::string name)
{
    return log4cpp::Category::getInstance(name);
}

    
