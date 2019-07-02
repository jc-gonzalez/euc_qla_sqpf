/******************************************************************************
 * File:    wa.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.WorkArea
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
 *   Implement WorkArea class
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

#include "wa.h"
#include "tools.h"

#include <sys/stat.h>

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
WorkArea::WorkArea()
{}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
WorkArea::WorkArea(string _wa)
{
    wa            = _wa;

    procArea      = wa + "/bin";

    localInbox    = wa + "/data/inbox";
    localOutputs  = wa + "/data/outbox";

    archive       = wa + "/data/archive";

    reproc        = wa + "/data/reproc";

    serverBase    = wa + "/server";

    remoteOutputs = wa + "/server/outputs";
    remoteInbox   = wa + "/server/inbox";

    run           = wa + "/run";
    runTools      = wa + "/run/bin";

    sessionId     = timeTag();

    sessionDir    = wa + "/run/" + sessionId;
    tasks         = sessionDir + "/tsk";
    logs          = sessionDir + "/log";

    for (auto & p: {sessionDir, tasks, logs}) {
        if (mkdir(p.c_str(), PathMode) < 0) {
            std::cerr << "Couldn't create folder "
                      << p << ": " << strerror(errno) << '\n';
            abort();
        }
    }
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
WorkArea::~WorkArea()
{
}

//----------------------------------------------------------------------
// Method: dump
//----------------------------------------------------------------------
void WorkArea::dump()
{
    std::cerr << "wa ...............:" << wa  << '\n'
              << "procArea .........:" << procArea  << '\n'
              << "localInbox .......:" << localInbox  << '\n'
              << "localOutputs .....:" << localOutputs  << '\n'
              << "archive ..........:" << archive  << '\n'
              << "reproc ...........:" << reproc  << '\n'
              << "serverBase .......:" << serverBase  << '\n'
              << "remoteOutputs ....:" << remoteOutputs  << '\n'
              << "remoteInbox ......:" << remoteInbox  << '\n'
              << "run ..............:" << run  << '\n'
              << "runTools .........:" << runTools  << '\n'
              << "sessionId ........:" << sessionId  << '\n'
              << "sessionDir .......:" << sessionDir  << '\n'
              << "tasks ............:" << tasks  << '\n'
              << "logs .............:" << logs  << '\n';
}
