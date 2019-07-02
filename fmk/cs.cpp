/******************************************************************************
 * File:    cs.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.ContainerSpectrum
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
 *   Implement ContainerSpectrum class
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

#include "cs.h"
#include <chrono>

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
ContainerSpectrum::ContainerSpectrum()
    : savedSpec({{"aborted", 0},
                  {"archived", 0},
                  {"failed", 0},
                  {"finished", 0},
                  {"paused", 0},
                  {"running", 0},
                  {"scheduled", 0},
                  {"stopped", 0}})
{
    cnts.setSize(40);
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
ContainerSpectrum::~ContainerSpectrum()
{
}

//----------------------------------------------------------------------
// Method: append
// Append container and status to the list
//----------------------------------------------------------------------
void ContainerSpectrum::append(string cont, string status)
{
    if (! cnts.find(cont)) {
        string oldCont = cnts.put(string(cont));
        if (!oldCont.empty()) {
            //std::cerr << "Appending " << cont
            //          << ", returned " << oldCont << '\n';
            auto cntIt = cntStatus.find(oldCont);
            string oldStatus = cntIt->second;
            savedSpec[oldStatus] = savedSpec[oldStatus] + 1;
            cntStatus.erase(cntIt);
        }
    }
    cntStatus[cont] = status;
}

//----------------------------------------------------------------------
// Method: spectrum
//----------------------------------------------------------------------
CntrSpectrum ContainerSpectrum::spectrum()
{
    CntrSpectrum spec(savedSpec);
    for (auto & kv: cntStatus) {
        spec[kv.second] = spec[kv.second] + 1;
    }
    return spec;
}
