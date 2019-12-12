/******************************************************************************
 * File:    ver_seqconf.h
 *          This file is part of Tools
 *
 * Domain:  tools.VersionSeqConf
 *
 * Last update:  1.0
 *
 * Date:    20191126
 *
 * Author:  J C Gonzalez
 *
 * Copyright (C) 2015-2020 Euclid SOC Team / J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare VersionSeqConf class
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

#ifndef VERSIONSEQCONF_H
#define VERSIONSEQCONF_H

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

//==========================================================================
// Class: VersionSeqConf
//==========================================================================
class VersionSeqConf {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    VersionSeqConf();

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~VersionSeqConf();

protected:

private:
    //----------------------------------------------------------------------
    // Method: init
    //----------------------------------------------------------------------
    virtual void init();

private:
    int id;
};

#endif // VERSIONSEQCONF_H
