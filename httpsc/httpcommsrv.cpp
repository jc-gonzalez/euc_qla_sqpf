/******************************************************************************
 * File:    httpcommsrv.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.HttpCommServer
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
 *   Implement HttpCommServer class
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

#include "httpcommsrv.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
HttpCommServer::HttpCommServer(int prt, string & pth)
    : port(prt), basePath(pth), cw(create_webserver(prt))
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
HttpCommServer::~HttpCommServer()
{
}

//----------------------------------------------------------------------
// Method: addRoute
//----------------------------------------------------------------------
void HttpCommServer::addRoute(webserver & ws, string route, http_resource * rscHdl)
{
    ws.register_resource(route, rscHdl);
}
