/******************************************************************************
 * File:    fv.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.FileVersion
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
 *   Declare FileVersion class
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

#ifndef FILEVERSION_H
#define FILEVERSION_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"

//==========================================================================
// Class: FileVersion
//==========================================================================
class FileVersion {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    FileVersion();
    FileVersion(unsigned int ma, unsigned int mi);
    FileVersion(std::string v);

public:
    unsigned int major();
    unsigned int minor();
    void setVersion(unsigned int ma, unsigned int mi);
    void setVersion(std::string s);
    void getVersion(unsigned int & ma, unsigned int & mi);
    std::string getVersion();
    void incr();
    void incrMajor();
    void incrMinor();

private:
    unsigned int mj, mn;
};


#endif // FILEVERSION_H
