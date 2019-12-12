/******************************************************************************
 * File:    httpcommsrv.h
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
 *   Declare HttpCommServer class
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

#ifndef HTTPCOMMSERVER_H
#define HTTPCOMMSERVER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>
using std::string;

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------

#include <httpserver.hpp>
using namespace httpserver;

typedef http_response                  HttpResp;
typedef std::shared_ptr<http_response> HttpRespPtr;
typedef string_response                strResp;

/*
class hello_world_resource : public http_resource {
public:
    const HttpRespPtr render_GET(const http_request&) {
        return HttpRespPtr(new strResp("GET: Hello, World!"));
    }

    const HttpRespPtr render(const http_request&) {
        return HttpRespPtr(new strResp("OTHER: Hello, World!"));
    }
};

class handling_multiple_resource : public http_resource {
public:
    const HttpRespPtr render(const http_request& req) {
        return HttpRespPtr(new strResp("Your URL: " + req.get_path()));
    }
};

class url_args_resource : public http_resource {
public:
    const HttpRespPtr render(const http_request& req) {
        return HttpRespPtr(new strResp("ARGS: " + req.get_arg("arg1") + " and " + req.get_arg("arg2")));
    }
};
*/
/*
int main(int argc, char** argv) {
    webserver ws = create_webserver(8080);

    hello_world_resource hwr;
    ws.register_resource("/hello", &hwr);

    handling_multiple_resource hmr;
    ws.register_resource("/family", &hmr, true);
    ws.register_resource("/with_regex_[0-9]+", &hmr);

    url_args_resource uar;
    ws.register_resource("/url/with/{arg1}/and/{arg2}", &uar);
    ws.register_resource("/url/with/parametric/args/{arg1|[0-9]+}/and/{arg2|[A-Z]+}", &uar);

    ws.start(true);

    return 0;
}
*/

//==========================================================================
// Class: HttpCommServer
//==========================================================================
class HttpCommServer {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    HttpCommServer(int prt, string & pth);

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~HttpCommServer();
  
protected:
    //----------------------------------------------------------------------
    // Method: addRoute
    //----------------------------------------------------------------------
    void addRoute(webserver & ws, string route, http_resource * rscHdl);

protected:
    int port;
    string basePath;
    create_webserver && cw;
};


#endif // HTTPCOMMSERVER_H
