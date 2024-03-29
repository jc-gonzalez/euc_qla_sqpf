/******************************************************************************
 * File:    cntrmng.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.ContainerMng
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
 *   Declare ContainerMng class
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

#ifndef CNTRMNG_H
#define CNTRMNG_H

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
#include "dckmng.h"

//==========================================================================
// Class: ContainerMng
//==========================================================================
class ContainerMng : public DockerMng {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ContainerMng(WorkArea & _wa);

    //----------------------------------------------------------------------
    // Method: createContainer
    // Creates a container that executes the requested application
    //----------------------------------------------------------------------
    virtual bool createContainer(std::string img, std::vector<std::string> opts,
                                 std::map<std::string, std::string> maps,
                                 std::string exe, std::vector<std::string> args,
                                 std::string & containerId, string & cmd_line);

    //----------------------------------------------------------------------
    // Method: createContainer
    // Creates a container that executes the requested application
    //----------------------------------------------------------------------
    virtual bool createContainer(std::string proc, std::string workDir,
                                 std::string & containerId);

    //----------------------------------------------------------------------
    // Method: getInfo
    // Retrieves information about running container
    //----------------------------------------------------------------------
    virtual bool getInfo(std::string id, std::stringstream & info);

    //----------------------------------------------------------------------
    // Method: kill
    // Kill running container
    //----------------------------------------------------------------------
    virtual bool kill(std::string id);

    //----------------------------------------------------------------------
    // Method: remove
    // Removes exited container
    //----------------------------------------------------------------------
    virtual bool remove(std::string id);

};

#endif  /* CNTRMNG_H */
