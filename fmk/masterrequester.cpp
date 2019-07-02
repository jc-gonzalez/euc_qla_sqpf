/******************************************************************************
 * File:    masterrequester.cpp
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
 *   Implement MasterRequester class
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

#include "masterrequester.h"

#include "rwc.h"
#include "str.h"
#include "filetools.h"
using namespace FileTools;

#include "scopeexit.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
MasterRequester::MasterRequester() : rwcHdl(new RWC)
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
MasterRequester::~MasterRequester()
{
}

//----------------------------------------------------------------------
// Method: setServerUrl
// Sets the server URL
//----------------------------------------------------------------------
void MasterRequester::setServerUrl(string srvUrl)
{
    serverUrl = srvUrl;
}

//----------------------------------------------------------------------
// Method: requestData
// Sets the server URL
//----------------------------------------------------------------------
bool MasterRequester::requestData(string route, string & resp,
                                  string contentType)
{
    // rwcHdl->setAuth(user, pwd);
    std::string downloadUrl = serverUrl + route;
    string result;
    rwcHdl->getContent(downloadUrl, resp, result);
    return (result.find("200 OK") != std::string::npos);
}

//----------------------------------------------------------------------
// Method: postFile
// Uses POST to send a file to a server
//----------------------------------------------------------------------
bool MasterRequester::postFile(string route, string fileName,
                               string contentType)
{
    string baseName = str::getBaseName(fileName);
    
    std::string uploadUrl = serverUrl + route + "/" + baseName;
    std::string result;
//     //std::cerr << uploadUrl << ", " << fileName << '\n';
    
    rwcHdl->postFile(uploadUrl, fileName, result, contentType);
//     //std::cerr << result << '\n';
    return (result.find("200 OK") != std::string::npos);
}

