/******************************************************************************
 * File:    cntrmng.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.cntrmng
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
 *   Implement ContainerMng class
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

#include "cntrmng.h"

#include "process.h"
#include "str.h"
#include "tools.h"

#include <cassert>
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ContainerMng::ContainerMng(WorkArea & _wa) : DockerMng(_wa) 
{
}

//----------------------------------------------------------------------
// Method: createContainer
// Creates a container that executes the requested application
//----------------------------------------------------------------------
bool ContainerMng::createContainer(std::string img, std::vector<std::string> opts,
                                   std::map<std::string, std::string> maps,
                                   std::string exe, std::vector<std::string> args,
                                   std::string & containerId, string & cmd_line)
{
    static char fileIdTpl[] = "dockerId_XXXXXX";

    procxx::process cnt("/usr/bin/docker", "run");
    for (auto & o : {"--detach", "--publish-all", "--privileged=true"}) { cnt.add_argument(o); }
    for (auto & o : opts) { cnt.add_argument(o); }
    for (auto & kv : maps) {
        cnt.add_argument("-v");
        cnt.add_argument(kv.first + ":" + kv.second);
    }

    //cnt.add_argument("-u");
    //cnt.add_argument("eucops");
    
    std::string tmpFileName("docker.id");
    //(void)mkTmpFileName(fileIdTpl, tmpFileName);

    //cnt.add_argument("--cidfile");
    //cnt.add_argument(tmpFileName);

    cnt.add_argument(img);

    cnt.add_argument(exe);
    for (auto & a : args) { cnt.add_argument(a); }

    cmd_line = cnt.cmd_line();
    
    cnt.exec();

    std::stringstream info("");
    std::string line;
    while (std::getline(cnt.output(), line)) {
        info << line << std::endl;
        if (!cnt.running() ||
            !procxx::running(cnt.id()) ||
            !running(cnt)) {
            break;
        }
    }
    std::cerr << "CNT: " << info.str() << '\n';
    cnt.wait();

    std::ofstream dockerIdFile(tmpFileName);
    dockerIdFile << info.str() << '\n';
    dockerIdFile.close();
    
    containerId = info.str();
    containerId.pop_back();

    std::cerr << "CONTAINER CODE: " << cnt.code() << '\n';
    return (cnt.code() == 0);
}

//----------------------------------------------------------------------
// Method: createContainer
// Creates a container that executes the requested application
//----------------------------------------------------------------------
bool ContainerMng::createContainer(std::string proc, std::string workDir,
                                   std::string & containerId)
{
    std::cerr << "CONTAINER CREATION: " << proc << ", " << workDir << '\n';

    std::string RunProcessorApp("python");
    std::string RunProcessorScript(wa.runTools + "/RunProcessor.py");

    std::string cfgFileArg(workDir + "/" + proc + ".cfg");
    std::string dockerIdFile(workDir + "/docker.id");

    //std::ofstream ofs;
    //ofs.open(cfgFileArg, std::ofstream::out);
    //ofs << getuid() << '\n';
    //ofs.close();

    procxx::process cnt(RunProcessorApp, RunProcessorScript,
                        "-t", workDir, "-c", cfgFileArg);

    std::cerr << "CONTAINER CMD: " << cnt.cmd_line() << '\n';

    cnt.exec();
    cnt.wait();

    while (containerId.empty()) {
        std::ifstream dockerIdStrm(dockerIdFile);
        std::getline(dockerIdStrm, containerId);
    }
    
    std::cerr << "CONTAINER CODE: " << cnt.code() << '\n';
    return (cnt.code() == 0);
}

//----------------------------------------------------------------------
// Method: getInfo
// Retrieves information about running container
//----------------------------------------------------------------------
bool ContainerMng::getInfo(std::string id, std::stringstream & info)
{
    procxx::process cntInspect("/usr/bin/docker", "inspect");
    cntInspect.add_argument(id);
    std::string fmt = info.str();
    if (fmt.length() > 0) {
        cntInspect.add_argument("--format");
        cntInspect.add_argument(fmt);
    }
    cntInspect.exec();

    std::cerr << "CONTAINER CMD: " << cntInspect.cmd_line() << '\n';

    info.str("");
    std::string line;
    while (std::getline(cntInspect.output(), line)) {
        info << line << std::endl;
        if (!cntInspect.running() ||
            !procxx::running(cntInspect.id()) ||
            !running(cntInspect)) {
            break;
        }
    }
    std::string completeInfo = info.str().substr(1);
    completeInfo.pop_back();
    info.str(completeInfo.substr(0, completeInfo.find_last_of("'")));

    std::cerr << "CONTAINER RETURN: " << info.str() << '\n';

    cntInspect.wait();
    std::cerr << "CONTAINER CODE: " << cntInspect.code() << '\n';
    return (cntInspect.code() == 0);
}

//----------------------------------------------------------------------
// Method: kill
// Kill a running container
//----------------------------------------------------------------------
bool ContainerMng::kill(std::string id)
{
    if (id.length() < 1) {
        procxx::process cntPs("docker", "ps", "-a");
        cntPs.exec();

        std::string line;
        std::ofstream myfile;
        myfile.open("/home/eucops/.qpf/dck.log", std::fstream::out | std::fstream::app); 
        while (std::getline(cntPs.output(), line)) {
            myfile << line << std::endl;
            if (!cntPs.running() ||
                !procxx::running(cntPs.id()) ||
                !running(cntPs)) { break; }
        }
        cntPs.wait();
        return (cntPs.code() == 0);
    }
    
    procxx::process cntKill("docker", "kill");
    {
      std::ofstream myfile;
      myfile.open("/home/eucops/.qpf/dck.log", std::fstream::out | std::fstream::app); 
      myfile << "Trying to kill container: docker kill " << id << '\n';
    }
    cntKill.add_argument(id);
    cntKill.exec();
    cntKill.wait();
    return (cntKill.code() == 0);
}

//----------------------------------------------------------------------
// Method: remove
// Remove a stopped container
//----------------------------------------------------------------------
bool ContainerMng::remove(std::string id)
{
    procxx::process srvRm("docker", "rm");
    {
      std::ofstream myfile;
      myfile.open("/home/eucops/.qpf/dck.log", std::fstream::out | std::fstream::app); 
      myfile << "Trying to remove container: docker rm " << id << '\n';
    }
    srvRm.add_argument(id);
    srvRm.exec();
    srvRm.wait();
    return (srvRm.code() == 0);
}

