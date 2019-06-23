/******************************************************************************
 * File:    masterserver.cpp
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
 *   Implement MasterServer class
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

#include "masterserver.h"
#include "master.h"

#include <memory>
#include <thread>

/*
------------------------------------------------------------
# List of routes handled by this server

This is the list of routes handled by this server, along with 
the methods (HTTP Verbs) defined specifically to handle the
communication through that route:

- /hello (GET) 
  Just a sample route, to see if server is launched

- /status (GET)
  Provides access from the clients to the Host information 
  structure 

- /


------------------------------------------------------------
*/

class RscHelloWorld : public http_resource {
public:
    const HttpRespPtr render_GET(const http_request&) {
        return HttpRespPtr(new strResp("GET: Hello, World!"));
    }

    const HttpRespPtr render(const http_request&) {
        return HttpRespPtr(new strResp("OTHER: Hello, World!"));
    }
};

class RscMultiple : public http_resource {
public:
    const HttpRespPtr render(const http_request& req) {
        return HttpRespPtr(new strResp("Your URL: " + req.get_path()));
    }
};

class RscArgs : public http_resource {
public:
    const HttpRespPtr render(const http_request& req) {
        return HttpRespPtr(new strResp("ARGS: " + req.get_arg("arg1") +
				       " and " + req.get_arg("arg2")));
    }
};

class RscHostStatus : public http_resource {
public:
    void setMasterHdl(Master * hdl) { mhdl = hdl; }
    
    const HttpRespPtr render_GET(const http_request&) {
        return HttpRespPtr(new strResp(mhdl->getHostInfo(), 200,
				       "application/json"));
    }

    const HttpRespPtr render(const http_request&) {
        return HttpRespPtr(new strResp("", 404));
    }
private:
    Master * mhdl;
};

class RscPostReceiver : public http_resource {
public:
    void setMasterHdl(Master * hdl) { mhdl = hdl; }
    
    const HttpRespPtr render_POST(const http_request&rqst) {
	std::vector<std::string> pathItems = rqst.get_path_pieces();
	std::cerr << rqst.content_too_large() << '\n';
	std::cerr << rqst << "\n\n";
	for (auto & p : pathItems) { std::cerr << p << " | "; }
	std::cerr << '\n';
	//std::cerr << "Content:\n";
	//std::cerr << rqst.get_content() << '\n';
        return HttpRespPtr(new strResp("Done.", 200));
    }

    const HttpRespPtr render(const http_request&) {
        return HttpRespPtr(new strResp("", 404));
    }
private:
    Master * mhdl;
};

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
MasterServer::MasterServer(Master * hdl, int prt, string & pth)
    : HttpCommServer(prt, pth), mhdl(hdl)
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
MasterServer::~MasterServer()
{
}

//----------------------------------------------------------------------
// Method: launch
// Initialize and launch the HTTP server
//----------------------------------------------------------------------
void MasterServer::launch()
{
    std::thread(&MasterServer::run, this).detach();
}

//----------------------------------------------------------------------
// Method: launch
// Initialize and launch the HTTP server
//----------------------------------------------------------------------
void MasterServer::run()
{
    webserver ws = create_webserver(port)
	.content_size_limit(45069760); // cw;

    RscHelloWorld rscHello;
    addRoute(ws, "/hello", &rscHello);

    RscHostStatus rscStatus;
    rscStatus.setMasterHdl(mhdl);
    addRoute(ws, "/status", &rscStatus);

    RscPostReceiver rscPostRcv;
    addRoute(ws, "/inbox/{prod}", &rscPostRcv);

    ws.start(true);
}

