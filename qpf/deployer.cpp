/******************************************************************************
 * File:    deployer.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.QPF.Deployer
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
 *   Implement Deployer class
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

#include "deployer.h"

#include <iostream>
#include <string>
#include <thread>

#include <unistd.h>

#include "master.h"

#include "log.h"

#define MINOR_SYNC_DELAY_MS    500

#define DEFAULT_INITIAL_PORT   50000

Deployer * deployerCatcher;

//----------------------------------------------------------------------
// Ancillary signal handler
//----------------------------------------------------------------------
void signalCatcher(int s)
{
    switch (s) {
    case SIGTERM:
        deployerCatcher->actionOnClosingSignal();
        break;
    default:     // Do nothing for other signals trapped (if any)
        break;
    }
}

//----------------------------------------------------------------------
// Constructor: Deployer
//----------------------------------------------------------------------
Deployer::Deployer(int argc, char *argv[])
    : verbosityLevel(2),
      logger(Log::getRootLogger())
{
    // Get host info
    setPort(DEFAULT_INITIAL_PORT);

    //== Change value for delay between peer nodes launches (default: 50000us)
    if ((!processCmdLineOpts(argc, argv)) || (cfgFileName.empty())) {
        usage(EXIT_FAILURE);
    }
}

//----------------------------------------------------------------------
// Destructor: Deployer
//----------------------------------------------------------------------
Deployer::~Deployer()
{
}


//----------------------------------------------------------------------
// Method: run
// Launches the system components and starts the system
//----------------------------------------------------------------------
int Deployer::run()
{
    static const int msDckWatchDogLapse = 5000;

    // Greetings...
    sayHello();

    //== Install signal handler
    deployerCatcher = this;
    installSignalHandlers();

    Master master(cfgFileName, currentHostAddr, port, workArea, balanceMode);
    master.run();
        
    // Bye, bye
    logger.info("Done.");
    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
// Method: usage
// Shows usage information
//----------------------------------------------------------------------
bool Deployer::usage(int code)
{
    std::cout << "Usage: " << exeName << "  -c configFile "
              << "[ -p initialPort ] [-i id ] [ -v ] [ -h ]\n"
              << "where:\n"
              << "\t-c cfgFile          System is reconfigured with configuration in\n"
              << "\t                    file cfgFile (configuration is then saved to DB).\n"
              << "\t-i id               Set the host identifier (name.domain or IP), it should\n"
              << "\t                    be one of the specified in the cfg. file.\n"
              << "\t-p portNum          Port number to use for server\n"
              << "\t-w workAreaDir      Sets the work area root folder\n"
              << "\t-b balanceMode      Sets the balancing mode between nodes\n"
              << "\t                    0:Sequential; 1:Load balance (default), 2:Random\n"
              << "\t-v                  Increases verbosity (default:silent operation).\n\n"
              << "\t-h                  Shows this help message.\n";

    exit(code);
}

//----------------------------------------------------------------------
// Method: processCmdLineOpts
// Processes command line options to configure execution
//----------------------------------------------------------------------
bool Deployer::processCmdLineOpts(int argc, char * argv[])
{
    bool retVal = true;
    int exitCode = EXIT_FAILURE;

    exeName = string(argv[0]);

    int opt;
    while ((opt = getopt(argc, argv, "hvp:c:i:w:b:")) != -1) {
        switch (opt) {
        case 'v':
            verbosityLevel++;
            break;
        case 'c':
            setConfigFile(string(optarg));
            break;
        case 'p':
            setPort(atoi(optarg));
            break;
        case 'i':
            setCurrentHostAddr(string(optarg));
            break;
        case 'w':
            setWorkArea(string(optarg));
            break;
        case 'b':
            setBalanceMode(atoi(optarg));
            break;
        case 'h':
            exitCode = EXIT_SUCCESS;
        default: /* '?' */
            usage(exitCode);
        }
    }

    return retVal;
}

//----------------------------------------------------------------------
// Method: setConfigFile
// Sets the name of the configuration file to be used
//----------------------------------------------------------------------
void Deployer::setConfigFile(string fName)
{
    cfgFileName = fName;
}

//----------------------------------------------------------------------
// Method: setPort
// Sets the server port for communications set up
//----------------------------------------------------------------------
void Deployer::setPort(int prt)
{
    port = prt;
}

//----------------------------------------------------------------------
// Method: setCurrentHostAddr
// Set the address (name.domain or IP) of the current host
//----------------------------------------------------------------------
void Deployer::setCurrentHostAddr(string addr)
{
    currentHostAddr = addr;
}

//----------------------------------------------------------------------
// Method: setWorkArea
// Sets the work area root folder
//----------------------------------------------------------------------
void Deployer::setWorkArea(string wa)
{
    workArea = wa;
}

//----------------------------------------------------------------------
// Method: setBalanceMode
// Sets the mode of load balancing between nodes
//----------------------------------------------------------------------
void Deployer::setBalanceMode(int mode)
{
    balanceMode = mode;
}

//----------------------------------------------------------------------
// Method: delay
// Waits for a small time lapse for system sync
//----------------------------------------------------------------------
int Deployer::delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

//----------------------------------------------------------------------
// Method: sayHello
// Shows a minimal title and build id for the application
//----------------------------------------------------------------------
void Deployer::sayHello()
{
    string buildId(BUILD_ID);
    if (buildId.empty()) {
        char buf[20];
        sprintf(buf, "%ld", (long)(time(0)));
        buildId = string(buf);
    }

    std::chrono::time_point<std::chrono::system_clock>
        now = std::chrono::system_clock::now();
    std::time_t the_time = std::chrono::system_clock::to_time_t(now);

    const string hline("----------------------------------------"
                       "--------------------------------------");

    logger << log4cpp::Priority::INFO << hline;
    logger << log4cpp::Priority::INFO << APP_NAME << " - " << APP_LONG_NAME;
    logger << log4cpp::Priority::INFO << APP_DATE << " - " << APP_RELEASE
           << " Build " << buildId ;
    logger << log4cpp::Priority::INFO << "Running at " << currentHostAddr
           << ", HTTP server listening at port " << port;
    logger << log4cpp::Priority::INFO << "Working area located at " << workArea;
    if (balanceMode > -1) {
        logger << log4cpp::Priority::INFO << "Node balancing mode is set to '"
            << BalancingModeStr[balanceMode] << "'";
    }
    logger << log4cpp::Priority::INFO << hline;
}

//----------------------------------------------------------------------
// Method: actionOnClosingSignal
// Actions to be performed when a closing signal is catched
//----------------------------------------------------------------------
void Deployer::actionOnClosingSignal()
{
    // Quit QPF Core (soft shutdown)
    //if (isMasterHost) { masterNodeElems.evtMng->quit(); }
}

//----------------------------------------------------------------------
// Method: installSignalHandlers
// Install signal handlers
//----------------------------------------------------------------------
void Deployer::installSignalHandlers()
{
    struct sigaction sigIgnHandler;
    sigIgnHandler.sa_handler = SIG_IGN;
    sigemptyset(&sigIgnHandler.sa_mask);
    sigIgnHandler.sa_flags = 0;

    sigaction(SIGINT,  &sigIgnHandler, nullptr);

    struct sigaction sigActHandler;
    sigActHandler.sa_handler = signalCatcher;
    sigemptyset(&sigActHandler.sa_mask);
    sigActHandler.sa_flags = 0;

    sigaction(SIGTERM, &sigActHandler, nullptr);
}

