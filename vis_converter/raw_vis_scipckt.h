/******************************************************************************
 * File:    raw_vis_scipckt.h
 *          This file is part of Tools
 *
 * Domain:  tools.RAW_VIS_SciPacket
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
 *   Declare RAW_VIS_SciPacket class
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

#ifndef RAW_VIS_SCIPACKET_H
#define RAW_VIS_SCIPACKET_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "vis_base.h"

//namespace VIS {

//==========================================================================
// Class: RAW_VIS_SciPacket
//==========================================================================
class RAW_VIS_SciPacket {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    RAW_VIS_SciPacket();

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

    //----------------------------------------------------------------------
    // Method: setCompressionInfo
    //----------------------------------------------------------------------
    void setCompressionInfo(CompressionInfo & i) { comprInfo = i; }

    //----------------------------------------------------------------------
    // Method: uncompress
    //----------------------------------------------------------------------
    int uncompress(unsigned short pixelsPerBlock);

public:
    SpaceWirePacketTag hdrTag;
    OperationID operationId;
    CcdId ccd;
    UInt16 dataLength;
    CRC16 crcHdr;
    uint8_t * data;
    CRC16 crcData;
    SpaceWirePacketTag ftrTag;

    CompressionInfo comprInfo;

    unsigned int pixelsPerQRow;

    long      uncomprDataLen;
    uint8_t * uncomprPixels;
    long      uncomprBytes;
};

//}

#endif // RAW_VIS_SCIPACKET_H
