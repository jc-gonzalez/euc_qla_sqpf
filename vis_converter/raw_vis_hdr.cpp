/******************************************************************************
 * File:    raw_vis_hdr.cpp
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
 *   Implement RAW_VIS_Header class
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

#include "raw_vis_hdr.h"

//namespace VIS {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
RAW_VIS_Header::RAW_VIS_Header()
{
    tcparams = new TCParameters(TC_PARAMS_BYTES);
}

//----------------------------------------------------------------------
// Method: read
//----------------------------------------------------------------------
void RAW_VIS_Header::read(ifstream & fh)
{
    hdrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireROEPacketHeaderTag));

    operationId.read(fh); // Read OperationID
    comprInfo.read(fh); // Read CompressionInfo
    startTime.read(fh); // Read MJDate
    expDuration.read(fh); // Read ExposureDuration
    imgSize.read(fh); // Read UInt32
    verticalStart.read(fh); // Read UInt16
    verticalEnd.read(fh); // Read UInt16
    mduSize.read(fh); // Read UInt16
    endTime.read(fh); // Read MJDate
    seqId.read(fh); // Read UInt16
    cfg3dId.read(fh); // Read UInt16
    verSeqConf.read(fh); // Read VersionSeqConf
    readoutCount.read(fh); // Read UInt16
    cfgTables.read(fh); // Read ConfigTables
    tcparams->read(fh); // Read TCParameters
    aswVersion.read(fh); // Read UInt32
    rsuCfgStatus.read(fh); // Read UInt16
    crc.read(fh); // Read CRC16

    ftrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireROEPacketFooterTag));
}

//----------------------------------------------------------------------
// Method: info
//----------------------------------------------------------------------
string RAW_VIS_Header::info()
{
    stringstream ss("");
    ss << "------------------------------------------------------------\n"
       << "Space_Wire_Packet_Header: " << hdrTag.info() << '\n'
       << "Operation_Id (SOC Id)     " << operationId.info() << '\n'
       << "Compression_Info:         " << comprInfo.info() << '\n'
       << "Start_Time (MJD):         " << startTime.info() << '\n'
       << "Exposure_Duration:        " << expDuration.info() << '\n'
       << "Image_Size:               " << imgSize.info() << '\n'
       << "Vertical_Start:           " << verticalStart.info() << '\n'
       << "Vertical_End:             " << verticalEnd.info() << '\n'
       << "Mdu_Size:                 " << mduSize.info() << '\n'
       << "End_Time (MJD)            " << endTime.info() << '\n'
       << "Sequence_Id:              " << seqId.info() << '\n'
       << "Config3D_Id:              " << cfg3dId.info() << '\n'
       << "Version_SeqConf:          " << verSeqConf.info() << '\n'
       << "Readout_Count:            " << readoutCount.info() << '\n'
       << "Config_Tables:            " << cfgTables.info() << '\n'
       << "TC_Parameters:            " << tcparams->info() << '\n'
       << "ASW_Version:              " << aswVersion.info() << '\n'
       << "RSU_Cfg_Status:           " << rsuCfgStatus.info() << '\n'
       << "CRC_16:                   " << crc.info() << '\n'
       << "Space_Wire_Packet_Footer: " << ftrTag.info() << '\n'
       << "------------------------------------------------------------\n";
    return ss.str();

}

//----------------------------------------------------------------------
// Method: checkCRC
//----------------------------------------------------------------------
bool RAW_VIS_Header::checkCRC()
{
    return true;
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
RAW_VIS_Header_prev::RAW_VIS_Header_prev()
{
    tcparams = new TCParameters(TC_PARAMS_BYTES);
}

//----------------------------------------------------------------------
// Method: read
//----------------------------------------------------------------------
void RAW_VIS_Header_prev::read(ifstream & fh)
{
    hdrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireROEPacketHeaderTag));
    
    operationId.read(fh); // Read OperationID
    comprInfo.read(fh); // Read CompressionInfo
    startTime.read(fh); // Read MJDate
    expDuration.read(fh); // Read ExposureDuration
    imgSize.read(fh); // Read UInt32
    verticalStart.read(fh); // Read UInt16
    verticalEnd.read(fh); // Read UInt16
    mduSize.read(fh); // Read UInt16
    endTime.read(fh); // Read MJDate
    seqId.read(fh); // Read UInt16
    cfg3dId.read(fh); // Read UInt16
    verSeqConf.read(fh); // Read VersionSeqConf
    readoutCount.read(fh); // Read UInt16
    cfgTables.read(fh); // Read ConfigTables
    tcparams->read(fh); // Read TCParameters
    aswVersion.read(fh); // Read UInt16
    crc.read(fh); // Read CRC16

    ftrTag.read(fh); // Read SpaceWirePacketTag
    assert(ftrTag.is(SpaceWireROEPacketFooterTag));
}

//----------------------------------------------------------------------
// Method: info
//----------------------------------------------------------------------
string RAW_VIS_Header_prev::info()
{
    stringstream ss("");
    ss << "------------------------------------------------------------\n"
       << "Space_Wire_Packet_Header: " << hdrTag.info() << '\n'
       << "Operation_Id (SOC Id)     " << operationId.info() << '\n'
       << "Compression_Info:         " << comprInfo.info() << '\n'
       << "Start_Time (MJD):         " << startTime.info() << '\n'
       << "Exposure_Duration:        " << expDuration.info() << '\n'
       << "Image_Size:               " << imgSize.info() << '\n'
       << "Vertical_Start:           " << verticalStart.info() << '\n'
       << "Vertical_End:             " << verticalEnd.info() << '\n'
       << "Mdu_Size:                 " << mduSize.info() << '\n'
       << "End_Time (MJD)            " << endTime.info() << '\n'
       << "Sequence_Id:              " << seqId.info() << '\n'
       << "Config3D_Id:              " << cfg3dId.info() << '\n'
       << "Version_SeqConf:          " << verSeqConf.info() << '\n'
       << "Readout_Count:            " << readoutCount.info() << '\n'
       << "Config_Tables:            " << cfgTables.info() << '\n'
       << "TC_Parameters:            " << tcparams->info() << '\n'
       << "ASW_Version:              " << aswVersion.info() << '\n'
       << "CRC_16:                   " << crc.info() << '\n'
       << "Space_Wire_Packet_Footer: " << ftrTag.info() << '\n'
       << "------------------------------------------------------------\n";
    return ss.str();

}

//----------------------------------------------------------------------
// Method: checkCRC
//----------------------------------------------------------------------
bool RAW_VIS_Header_prev::checkCRC()
{
    return true;
}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
RAW_VIS_Header_old::RAW_VIS_Header_old()
{
}

//----------------------------------------------------------------------
// Method: read
//----------------------------------------------------------------------
void RAW_VIS_Header_old::read(ifstream & fh)
{
    hdrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireROEPacketHeaderTag));
    
    operationId.read(fh); // Read OperationID
    comprInfo.read(fh); // Read CompressionInfo
    startTime.read(fh); // Read MJDate
    expDuration.read(fh); // Read ExposureDuration
    imgSize.read(fh); // Read UInt32
    verticalStart.read(fh); // Read UInt16
    verticalEnd.read(fh); // Read UInt16
    mduSize.read(fh); // Read UInt16
    spare.read(fh); // Read spare bytes
    crc.read(fh); // Read CRC16

    ftrTag.read(fh); // Read SpaceWirePacketTag
    assert(hdrTag.is(SpaceWireROEPacketFooterTag));
}

//----------------------------------------------------------------------
// Method: info
//----------------------------------------------------------------------
string RAW_VIS_Header_old::info()
{
    stringstream ss("");
    ss << "------------------------------------------------------------\n"
       << "Space_Wire_Packet_Header: " << hdrTag.info() << '\n'
       << "Operation_Id (SOC Id):    " << operationId.info() << '\n'
       << "Compression_Info:         " << comprInfo.info() << '\n'
       << "Start_Time (MJD):         " << startTime.info() << '\n'
       << "Exposure_Duration:        " << expDuration.info() << '\n'
       << "Image_Size:               " << imgSize.info() << '\n'
       << "Vertical_Start:           " << verticalStart.info() << '\n'
       << "Vertical_End:             " << verticalEnd.info() << '\n'
       << "Mdu_Size:                 " << mduSize.info() << '\n'
       << "CRC_16:                   " << crc.info() << '\n'
       << "Space_Wire_Packet_Footer: " << ftrTag.info() << '\n'
       << "------------------------------------------------------------\n";
    return ss.str();
}

//----------------------------------------------------------------------
// Method: checkCRC
//----------------------------------------------------------------------
bool RAW_VIS_Header_old::checkCRC()
{
    return true;
}

//}
