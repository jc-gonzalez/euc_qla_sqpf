/******************************************************************************
 * File:    metadatareader.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.MetadataReader
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
 *   Declare MetadataReader class
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

#ifndef METADATAREADER_H
#define METADATAREADER_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "str.h"
#include "types.h"

//==========================================================================
// Class: MetadataReader
//==========================================================================
class MetadataReader {
public:
    MetadataReader();
    MetadataReader(std::string & fName);

    virtual bool setFile(std::string & fName);
    virtual bool getMetadataInfoStr(std::string & md);
    virtual bool getMetadataInfo(json & md);

protected:
    std::string fileName;
};

//==========================================================================
// Class: FitsMetadataReader
//==========================================================================
class FitsMetadataReader : public MetadataReader {

public:
    FitsMetadataReader();
    FitsMetadataReader(std::string & fName);

    virtual bool setFile(std::string & fName);
    virtual bool getMetadataInfoStr(std::string & md);
    virtual bool getMetadataInfo(json & md);

protected:
    bool readFitsMetadata(std::string & str);
    void clearComment(std::string & line); 
    void clearQuotes(std::string & line);
};

#endif // METADATAREADER_H
