/******************************************************************************
 * File:    cs.h
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
 *   Declare ContainerSpectrum class
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

#ifndef CONTAINERSPECTRUM_H
#define CONTAINERSPECTRUM_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"
#include "fifo.h"

//==========================================================================
// Class: ContainerSpectrum
//==========================================================================
class ContainerSpectrum {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ContainerSpectrum();

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~ContainerSpectrum();

public:
    //----------------------------------------------------------------------
    // Method: append
    //----------------------------------------------------------------------
    void append(string cont, string status);

    //----------------------------------------------------------------------
    // Method: spectrum
    //----------------------------------------------------------------------
    CntrSpectrum spectrum();

private:
    FiFo<string> cnts;
    map<string, string> cntStatus;
    CntrSpectrum savedSpec;
};

#endif // CONTAINERSPECTRUM_H
