/******************************************************************************
 * File:    jsonfhdl.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.jsonfhdl
 *
 * Version:  2.0
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2018 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement JsonFileHandler class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include "jsonfhdl.h"

#include <fstream>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//----------------------------------------------------------------------
// Method: read
// Reads the file and stores the retrieved data internally
//----------------------------------------------------------------------
bool JsonFileHandler::read()
{
    std::ifstream jsonFile(fileName);
    jsonFile >> data;
    return true;
}

//----------------------------------------------------------------------
// Method: write
// Write the internal data in the file
//----------------------------------------------------------------------
bool JsonFileHandler::write()
{
    std::ofstream jsonFile;
    jsonFile.open(fileName, std::ofstream::out);
    if (!jsonFile) { return false; }
    jsonFile << data << '\n';
    return true;
}

//----------------------------------------------------------------------
// Method: setData
// Sets the value of the internal data container
//----------------------------------------------------------------------
void JsonFileHandler::setData(json & v)
{
    data = v;
}

//----------------------------------------------------------------------
// Method: getData
// Return handler for internal data
//----------------------------------------------------------------------
json & JsonFileHandler::getData()
{
    return data;
}


// }
