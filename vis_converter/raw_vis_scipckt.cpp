/******************************************************************************
 * File:    raw_vis_scipckt.cpp
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
 *   Implement RAW_VIS_SciPacket class
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

#include "raw_vis_scipckt.h"

//namespace VIS {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
RAW_VIS_SciPacket::RAW_VIS_SciPacket()
    : data(nullptr), uncomprPixels(nullptr)
{
}

//----------------------------------------------------------------------
// Method: read
//----------------------------------------------------------------------
void RAW_VIS_SciPacket::read(ifstream & fh)
{
    hdrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireSciPacketHeaderTag));

    operationId.read(fh);       // Read OperationID
    ccd.read(fh); // Read CompressionInfo
    dataLength.read(fh);           // Read MJDate
    crcHdr.read(fh);            // Read CRC of header

    if (data != nullptr) { delete [] data; }
    data = new uint8_t [dataLength.value];
    fh.read((char *)data, size_t(dataLength.value) * sizeof(uint8_t));

    crcData.read(fh); // Read CRC of data

    ftrTag.read(fh);            // Read SpaceWirePacketTag
    assert(ftrTag.is(SpaceWireSciPacketFooterTag));

    // Uncompress compressed data
    int nFinalBytes = dataLength.value;
    if (comprInfo.compr_type != NO_COMPR) {
        nFinalBytes = uncompress(comprInfo.compr_prs);
    }
}

//----------------------------------------------------------------------
// Method: info
//----------------------------------------------------------------------
string RAW_VIS_SciPacket::info()
{
    stringstream ss("");
    ss << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n"
       << "Space_Wire_Packet_Header: " << hdrTag.info() << '\n'
       << "Operation_Id (SOC Id):    " << operationId.info() << '\n'
       << "CCD ID:                   " << ccd.info() << '\n'
       << "Data length:              " << dataLength.info() << " bytes\n"
       << "CRC16 Header:             " << crcHdr.info() << '\n'
       << "CRC16 Data:               " << crcData.info() << '\n'
       << "Space_Wire_Packet_Header: " << ftrTag.info() << '\n'
       << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n";
    return ss.str();
}

//----------------------------------------------------------------------
// Method: uncompress
// Uncompressed the data read
//----------------------------------------------------------------------
int RAW_VIS_SciPacket::uncompress(unsigned short pixelsPerBlock)
{
    uint8_t * comprData = data;
    long comprDataLen = dataLength.value;

    pixelsPerQRow  = 2128;

    uncomprDataLen = pixelsPerQRow * bytesPerPixel;
    uncomprBytes = 0;

    if (uncomprPixels != nullptr) { delete [] uncomprPixels; }
    uncomprPixels = new uint8_t [uncomprDataLen];

    uncomprBytes = szip_uncompress_memory(szipOptMask,
                                          bitsPerPixel,
                                          pixelsPerBlock,
                                          pixelsPerQRow,
                                          (const char*)comprData,
                                          comprDataLen,
                                          (void *)uncomprPixels,
                                          uncomprDataLen);

    if (uncomprBytes < 0) {
        std::cerr << "ERROR: szip_uncompress_memory error\n";
        exit(EXIT_FAILURE);
    }

    return uncomprBytes;
}

//}
