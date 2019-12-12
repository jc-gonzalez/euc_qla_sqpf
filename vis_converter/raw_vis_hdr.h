/******************************************************************************
 * File:    raw_vis_hdr.h
 *          This file is part of Tools
 *
 * Domain:  tools.RAW_VIS_Header
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
 *   Declare RAW_VIS_Header class
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

#ifndef RAW_VIS_HEADER_H
#define RAW_VIS_HEADER_H

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
#include "vis_base.h"

//namespace VIS {

//==========================================================================
// Class: RAW_VIS_Header
// Base class for all RAW VIS Header classes
//==========================================================================
class RAW_VIS_Header_base {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    RAW_VIS_Header_base() {}

    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    virtual ~RAW_VIS_Header_base() {}

    //----------------------------------------------------------------------
    // Method: read
    //----------------------------------------------------------------------
    virtual void read(ifstream & fh) = 0;

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    virtual string info() = 0;

    //----------------------------------------------------------------------
    // Method: checkCRC
    //----------------------------------------------------------------------
    virtual bool checkCRC() = 0;

public:
    SpaceWirePacketTag hdrTag;
    OperationID operationId;
    CompressionInfo comprInfo;
    MJDate startTime;
    UInt32 imgSize;
    MJDate endTime;
    CRC16 crc;
    SpaceWirePacketTag ftrTag;
};

//==========================================================================
// Class: RAW_VIS_Header
// Compliant with VIS ICD 4.0 draf 9
//==========================================================================
class RAW_VIS_Header : public RAW_VIS_Header_base {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    RAW_VIS_Header();

    //----------------------------------------------------------------------
    // Method: read
    //----------------------------------------------------------------------
    void read(ifstream & fh);

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    string info();

    //----------------------------------------------------------------------
    // Method: checkCRC
    //----------------------------------------------------------------------
    bool checkCRC();

public:
    ExposureDuration expDuration;
    UInt16 verticalStart;
    UInt16 verticalEnd;
    UInt16 mduSize;
    UInt16 seqId;
    UInt16 cfg3dId;
    VersionSeqConf verSeqConf;
    UInt16 readoutCount;
    ConfigTables cfgTables;
    TCParameters * tcparams;
    UInt32 aswVersion;
    UInt16 rsuCfgStatus;
};

//==========================================================================
// Class: RAW_VIS_Header_prev
// Compliant with VIS ICD 4.0 draf ?
//==========================================================================
class RAW_VIS_Header_prev : public RAW_VIS_Header_base {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    RAW_VIS_Header_prev();

    //----------------------------------------------------------------------
    // Method: read
    //----------------------------------------------------------------------
    void read(ifstream & fh);

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    string info();

    //----------------------------------------------------------------------
    // Method: checkCRC
    //----------------------------------------------------------------------
    bool checkCRC();

public:
    ExposureDuration expDuration;
    UInt16 verticalStart;
    UInt16 verticalEnd;
    UInt16 mduSize;
    UInt16 seqId;
    UInt16 cfg3dId;
    VersionSeqConf verSeqConf;
    UInt16 readoutCount;
    ConfigTables cfgTables;
    TCParameters * tcparams;
    UInt16 aswVersion; // UInt32
    // UInt16 rsuCfgStatus;
};

//==========================================================================
// Class: RAW_VIS_Header
// Compliant with VIS ICD 3.4
//==========================================================================
class RAW_VIS_Header_old : public RAW_VIS_Header_base {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    RAW_VIS_Header_old();

    //----------------------------------------------------------------------
    // Method: read
    //----------------------------------------------------------------------
    void read(ifstream & fh);

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    string info();

    //----------------------------------------------------------------------
    // Method: checkCRC
    //----------------------------------------------------------------------
    bool checkCRC();

public:
    ExposureDuration expDuration;
    UInt16 verticalStart;
    UInt16 verticalEnd;
    UInt16 mduSize;
    UInt32 spare;
};

//}

#endif // RAW_VIS_HEADER_H
