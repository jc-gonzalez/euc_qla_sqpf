/******************************************************************************
 * File:    dckmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.DockerMng
 *
 * Last update:  2.0
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
 *   Declare DockerMng class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
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

#ifndef DCKMNG_H
#define DCKMNG_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------
#include <vector>
#include <map>
#include <string>
#include <sstream>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------
#include "wa.h"

//==========================================================================
// Class: DockerMng
//==========================================================================
class DockerMng {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    DockerMng(WorkArea & _wa) : wa(_wa) {}

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~DockerMng() {}

    //----------------------------------------------------------------------
    // Method: createService
    // Creates a service that retrieves data from TskMng & processes them
    //----------------------------------------------------------------------
    virtual bool createService(std::string srv, std::string img, int numScale,
                               std::string exe, std::vector<std::string> args) {}

    //----------------------------------------------------------------------
    // Method: createContainer
    // Creates a container that executes the requested application
    //----------------------------------------------------------------------
    virtual bool createContainer(std::string img, std::vector<std::string> opts,
                                 std::map<std::string, std::string> maps,
                                 std::string exe, std::vector<std::string> args,
                                 std::string & containerId) {}

    //----------------------------------------------------------------------
    // Method: createContainer
    // Creates a container that executes the requested application
    //----------------------------------------------------------------------
    virtual bool createContainer(std::string proc, std::string workDir,
                                 std::string & containerId) {}

    //----------------------------------------------------------------------
    // Method: reScaleService
    // Rescales a running service
    //----------------------------------------------------------------------
    virtual bool reScaleService(std::string srv, int newScale) {}

    //----------------------------------------------------------------------
    // Method: getDockerInfo
    // Retrieves information about Docker running instance
    //----------------------------------------------------------------------
    virtual bool getDockerInfo(std::stringstream & info, std::string filt);

    //----------------------------------------------------------------------
    // Method: getInfo
    // Retrieves information about running service
    //----------------------------------------------------------------------
    virtual bool getInfo(std::string id, std::stringstream & info) {}

    //----------------------------------------------------------------------
    // Method: getInfo
    // Retrieves information about running service
    //----------------------------------------------------------------------
    virtual bool getInfo(std::string id, std::string & info);

    //----------------------------------------------------------------------
    // Method: kill
    // Kill running container
    //----------------------------------------------------------------------
    virtual bool kill(std::string id) {}

    //----------------------------------------------------------------------
    // Method: remove
    // Removes exited container
    //----------------------------------------------------------------------
    virtual bool remove(std::string id) {}

    //----------------------------------------------------------------------
    // Method: leaveSwarm
    // Make a node leave the swarm
    //----------------------------------------------------------------------
    virtual bool leaveSwarm(std::string & addr) {}

    //----------------------------------------------------------------------
    // Method: shutdown
    //Shutdown entire swarm
    //----------------------------------------------------------------------
    virtual bool shutdown(std::string srv) {}

    //----------------------------------------------------------------------
    // Method: getContainerList
    // Retrieves list of container ids in the form of a vector
    //----------------------------------------------------------------------
    virtual bool getContainerList(std::vector<std::string> & contList);

    //----------------------------------------------------------------------
    // Method: runCmd
    // Run Docker command with argument
    //----------------------------------------------------------------------
    virtual bool runCmd(std::string cmd, std::vector<std::string> args,
                        std::string & containerId);

protected:
    WorkArea & wa;
};

#endif  /* DCKMNG_H */
