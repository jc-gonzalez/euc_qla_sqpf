/******************************************************************************
 * File:    deployer.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.QPF.Deployer
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
 *   Declare Deployer class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   CommNode
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

#ifndef DEPLOYER_H
#define DEPLOYER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - csignal
//------------------------------------------------------------
#include <csignal>

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   - types.h
//   - version
//------------------------------------------------------------
#include "types.h"
#include "log.h"
#include "version.h"

#ifndef BUILD_ID
#define BUILD_ID ""
#endif

//==========================================================================
// Class: Deployer
//==========================================================================
class Deployer {

public:
    //----------------------------------------------------------------------
    // Constructor: Deployer
    //----------------------------------------------------------------------
    Deployer(int argc, char *argv[]);

    //----------------------------------------------------------------------
    // Destructor: Deployer
    //----------------------------------------------------------------------
    ~Deployer();

    //----------------------------------------------------------------------
    // Method: run
    // Launches the system components and starts the system
    //----------------------------------------------------------------------
    int run();

    //----------------------------------------------------------------------
    // Method: actionOnClosingSignal
    // Actions to be performed when a closing signal is catched
    //----------------------------------------------------------------------
    void actionOnClosingSignal();

private:
    //----------------------------------------------------------------------
    // Method: usage
    // Shows usage information
    //----------------------------------------------------------------------
    bool usage(int code);

    //----------------------------------------------------------------------
    // Method: processCmdLineOpts
    // Processes command line options to configure execution
    //----------------------------------------------------------------------
    bool processCmdLineOpts(int argc, char * argv[]);

    //----------------------------------------------------------------------
    // Method: setConfigFile
    // Sets the name of the configuration file to be used
    //----------------------------------------------------------------------
    void setConfigFile(string fName);

    //----------------------------------------------------------------------
    // Method: setPort
    // Sets the server port for communications set up
    //----------------------------------------------------------------------
    void setPort(int prt);

    //----------------------------------------------------------------------
    // Method: setCurrentHostAddr
    // Set the address (IP) of the current host
    //----------------------------------------------------------------------
    void setCurrentHostAddr(string addr);

    //----------------------------------------------------------------------
    // Method: setWorkArea
    // Sets the work area root folder
    //----------------------------------------------------------------------
    void setWorkArea(string wa);

    //----------------------------------------------------------------------
    // Method: setBalanceMode
    // Sets the mode of load balancing between nodes
    //----------------------------------------------------------------------
    void setBalanceMode(int mode);
 
    //----------------------------------------------------------------------
    // Method: readConfiguration
    // Retrieves the configuration for the execution of the system (from
    // a disk file or from the internal database);
    //----------------------------------------------------------------------
    void readConfiguration();

    //----------------------------------------------------------------------
    // Method: delay
    // Waits for a small time lapse for system sync
    //----------------------------------------------------------------------
    int delay(int ms);

    //----------------------------------------------------------------------
    // Method: sayHello
    // Shows a minimal title and build id for the application
    //----------------------------------------------------------------------
    void sayHello();

    //----------------------------------------------------------------------
    // Method: installSignalHandlers
    // Install signal handlers
    //----------------------------------------------------------------------
    void installSignalHandlers();

private:
    string            exeName;
    int               verbosityLevel;
    
    string            cfgFileName;
    string            currentHostAddr;
    int               port;

    string            workArea;
    int               balanceMode;

    Logger            logger;
};

//}

#endif  /* DEPLOYER_H */
