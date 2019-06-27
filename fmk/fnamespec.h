/******************************************************************************
 * File:    fnamespec.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.FileNameSpec
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
 *   Declare FileNameSpec class
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

#ifndef FILENAMESPEC_H
#define FILENAMESPEC_H

#define USE_CXX11_REGEX
#undef  USE_CXX11_REGEX

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

#ifdef USE_CXX11_REGEX
#include <regex>
#endif

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"

//==========================================================================
// Class: FileNameSpec
//==========================================================================
class FileNameSpec {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    FileNameSpec();

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~FileNameSpec();

public:
    //----------------------------------------------------------------------
    // Method: parse
    //----------------------------------------------------------------------
    bool parse(string & fullFileName, ProductMeta & meta);

private:
    //----------------------------------------------------------------------
    // Method: genProdFormat
    //----------------------------------------------------------------------
    std::string genProdFormat(string & ext);

    //----------------------------------------------------------------------
    // Method: parseInstance
    //----------------------------------------------------------------------
    void parseInstance(string inst, ProductMeta & meta);

#ifdef USE_CXX11_REGEX
#else
    //----------------------------------------------------------------------
    // Method: parse
    //----------------------------------------------------------------------
    bool parseSnameNoRE(string sname, string & mission, string & proc_func,
                        string & instance, string & datetime, string & version);

    //----------------------------------------------------------------------
    // Method: parse
    //----------------------------------------------------------------------
    bool fieldIsMadeOf(string & fld, string chars);
#endif // USE_CXX11_REGEX
    
    //----------------------------------------------------------------------
    // Method: retrieveInternalMetadata
    //----------------------------------------------------------------------
    void retrieveInternalMetadata(string fileName, ProductMeta & meta);
    
private:
    static const string BnameRe;
    static const string SpectralBands;
    static const string Creators;
    static const string DataTypes;
};

#endif // FILENAMESPEC_H
