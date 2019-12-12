/******************************************************************************
 * File:    vis_base.h
 *          This file is part of Tools
 *
 * Domain:  tools.VIS_Base
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
 *   Declare VIS_Base class
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

#ifndef VIS_BASE_H
#define VIS_BASE_H

//======================================================================
// Group: External Dependencies
//======================================================================

//----------------------------------------------------------------------
// Topic: System headers
//----------------------------------------------------------------------
#include <cstdlib>

//----------------------------------------------------------------------
// Topic: External packages
//----------------------------------------------------------------------
extern "C" {
#   include "szlib.h"
}

//----------------------------------------------------------------------
// Topic: Project headers
//----------------------------------------------------------------------
#include "types.h"

//namespace VIS {

extern const vector<string> SequenceIdName;

extern uint32_t MSK32;
extern uint32_t MSK16;
extern uint32_t MSK8;

#define TC_PARAMS_BYTES 74

// Compression/Decompression optionss mask
extern unsigned int szipOptMask;

extern unsigned int bitsPerPixel;
extern unsigned int bytesPerPixel;

// Input and Output types

#undef T

#define T_INPUT_TYPE                                                    \
    T(raw, "Binary file, compliant with ICD 4.0 draft 9"),              \
        T(raw_prev, "Binary file, compliant with ICD 4.0 previous"),    \
        T(raw_old, "Binary file, compliant with ICD 3.4"),              \
        T(le1, "LE1 VIS Product")

#define T(a, b)  IN_ ## a
enum InputType { T_INPUT_TYPE, IN_unknown=99 };
#undef T

extern vector<string> InputTypeTag;
extern map<string, string> InputTypeDesc;
extern map<string, InputType> InputTypeId;

#define T_OUTPUT_TYPE                                                   \
    T(le1, "LE1 VIS Product (quadrant based)"),                         \
        T(ccd, "Each layer contains a full CDD (4 quadrants)"),         \
        T(fpa, "Full FPA")

#define T(a, b)  OUT_ ## a
enum OutputType { T_OUTPUT_TYPE, OUT_unknown=99 };
#undef T

extern vector<string> OutputTypeTag;
extern map<string, string> OutputTypeDesc;
extern map<string, OutputType> OutputTypeId;

//----------------------------------------------------------------------
// Class: VisArgs
//----------------------------------------------------------------------
class VisArgs {
public:
    string iFile;
    string oFile;
    string iType;
    string oType;
};

//----------------------------------------------------------------------
// Class: VISSize
//----------------------------------------------------------------------
    class VISSize {
public:
    static const int ROWS;
    static const int COLS;
    static const int ROWS_HALF;
    static const int COLS_HALF;

    static const int ROWS_FPA;
    static const int COLS_FPA;
    static const int ROWS_QUAD;
    static const int COLS_QUAD;

    static const int QUAD_PRE_SCAN_COLS;
    static const int QUAD_OVER_SCAN_COLS;

    static const int CHARGE_INJEC_STRUCT_ROWS;
    static const int QUAD_CHARGE_INJEC_STRUCT_ROWS;
};

//----------------------------------------------------------------------
// Enum: ComprMode
// Compression Mode: 0: Science, 1: Manual
//----------------------------------------------------------------------
enum ComprMode {
    SCIENCE = 0,
    MANUAL = 1
};

//----------------------------------------------------------------------
// Enum: ComprType
// Compression Type:
// 0: No Compr
// 1: 121 Without reordering
// 2: 121 With reordering
//----------------------------------------------------------------------
enum ComprType {
   NO_COMPR = 0,
   WITHOUT_REORDER_121 = 1,
   WITH_REORDER_121 = 2,
};

//----------------------------------------------------------------------
// Class: SpaceWirePacketTag
// A start/end of packet tag
//----------------------------------------------------------------------
class SpaceWirePacketTag : public UInt32 {
public:
    SpaceWirePacketTag() {}
public:
    bool is(uint32_t expectedContent) { return expectedContent == value; }
    void rollback(ifstream & fh) { fh.seekg(-sizeof(data), fh.cur); }
};

//----------------------------------------------------------------------
// Class: OperationID
// Teh (composed) operation ID
//----------------------------------------------------------------------
class OperationID : public UInt32 {};

//----------------------------------------------------------------------
// Class: CRC16
// A 16-bit computed CRC
//----------------------------------------------------------------------
class CRC16 : public UInt16 {};

//----------------------------------------------------------------------
// Class: MJDate
// A 6-byte definition of the MJD
//----------------------------------------------------------------------
class MJDate : public UInt48 {};

//----------------------------------------------------------------------
// Class: CompressionInfo
// Contains fields with information on VIS RAW data compression
//----------------------------------------------------------------------
class CompressionInfo {
public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    CompressionInfo() : mode(SCIENCE), compr_type(NO_COMPR), compr_prs(0) {}

public:
    ComprMode mode;
    ComprType compr_type;
    uint16_t compr_prs;
    UInt16 compr_info;

public:
    //----------------------------------------------------------------------
    // Method: set
    // Set the different components of the Compression Info object,
    // or the entire set of bytes at once
    // Parameters:
    //   mode       - Sets the internal parameter mode
    //   compr_type - Sets the internal parameter compr_type
    //   compr_prs  - Sets the internal parameter compr_prs
    //   compr_info - Sets the compount parameter (entire value)
    //----------------------------------------------------------------------
    void set(ComprMode amode, ComprType acompr_type, uint16_t acompr_prs,
             uint16_t acompr_info = 0xffff)
    {
        if (acompr_info == 0xffff) {
            mode = amode;
            compr_type = acompr_type;
            compr_prs = acompr_prs;
            compr_info.value = (((int(mode) & 0x00000003) << 14) |
                                ((int(compr_type) & 0x00000003) << 12) |
                                (compr_prs & 0x00000fff));
        } else {
            compr_info.value = acompr_info;
            mode = ComprMode((compr_info.value >> 14) & 0x00000003);
            compr_type = ComprType((compr_info.value >> 12) & 0x00000003);
            compr_prs = compr_info.value & 0x00000fff;
        }
    }

    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        compr_info.read(fh);
        set(ComprMode(0), ComprType(0), 0, compr_info.value);
        return true;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        static vector<string> comprTypeStr = {"No Compr",
                                              "121 Without reordering",
                                              "121 With reordering", ""};
        static char buffer[MAX_TMP_STR];

        sprintf(buffer, "%04X = Mode: %d - %s, ComprType: %d - %s, "
                "ComprPrs: %d px per block",
                compr_info.value,
                mode, (mode == SCIENCE) ? "SCIENCE" : "MANUAL",
                compr_type, comprTypeStr[int(compr_type)].c_str(), compr_prs);
        return string(buffer);
    }
};

//----------------------------------------------------------------------
// Class: ExposureDuration
// Contains fields with ROE Exposure duration fields
//----------------------------------------------------------------------
class ExposureDuration {

public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    ExposureDuration() {
    }

public:
    UInt64 expDuration[12];

public:
    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        bool retval = true;
        for (int i = 0; i < 12; ++i) { retval &= expDuration[i].read(fh); }
        return retval;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        stringstream ss("");
        for (int i = 0; i < 12; ++i) {
            ss << i << ":" << expDuration[i].value << "ms ";
        }
        return ss.str();
    }
};

//----------------------------------------------------------------------
// Class: VersionSeqConf
// Contains the configuration table version for the ref. ROE sequence
//----------------------------------------------------------------------
class VersionSeqConf {

public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    VersionSeqConf() {}

public:
    UInt16 verSeqConf[12];

public:
    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        bool retval = true;
        for (int i = 0; i < 12; ++i) { retval &= verSeqConf[i].read(fh); }
        return retval;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        stringstream ss("");
        for (int i = 0; i < 12; ++i) {
            ss << i << ":" << verSeqConf[i].value << "ms ";
        }
        return ss.str();
    }
};

//----------------------------------------------------------------------
// Class: ConfigTables
// Contains:
// - Config.Sine table ID: Table version of RSU SINE table used in the
//   RSU initialisation: default=1. Table version= 0 in case of RSU
//   not initialised
// - Config.Freq. table ID: Table version of RSU FREQ table used in
//   the RSU initialisation: default=2. Table version= 0 in case of
//   RSU not initialised
// - Config.Short Freq. table ID: Table version of RSU SHORT FREQ
//   table used in the RSU initialisation: default=1. Table version= 0
//   in case of RSU not initialised
//----------------------------------------------------------------------
class ConfigTables {

public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    ConfigTables() : cfgSine(1), cfgFreq(2), cfgShortFreq(1) {}

public:
    uint16_t cfgSine;
    uint16_t cfgFreq;
    uint16_t cfgShortFreq;

public:
    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        bool retval = true;
        UInt16 data;

        retval &= data.read(fh);
        if (retval) { cfgSine = data.value; }

        retval &= data.read(fh);
        if (retval) { cfgFreq = data.value; }

        retval &= data.read(fh);
        if (retval) { cfgShortFreq = data.value; }

        return retval;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        stringstream ss("");
        ss << cfgSine << ", " << cfgFreq << ", " << cfgShortFreq;
        return ss.str();
    }
};

//----------------------------------------------------------------------
// Class: TCParameters
// Contains the TC Parameters information
//----------------------------------------------------------------------
class TCParameters {

public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    TCParameters(int nbytes) {
        tcparams.resize(nbytes);
    }

public:
    vector<uint8_t> tcparams;

public:
    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        uint8_t data;
        for (int i = 0; i < tcparams.size(); ++i) {
            fh.read((char*)&data, sizeof(data));
            tcparams[i] = data;
        }
        return true;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        return "[...]";
    }
};

//----------------------------------------------------------------------
// Class: CcdId
// Contains fields with information on VIS RAW data compression
//----------------------------------------------------------------------
class CcdId {

public:
    //----------------------------------------------------------------------
    // Constructor
    // Initialize class instance
    //----------------------------------------------------------------------
    CcdId() : ccd_id(0), col(0), row(0) {}

public:
    uint16_t ccd_id;
    uint16_t col;
    uint16_t row;
    UInt32 data;

public:
    //----------------------------------------------------------------------
    // Method: set
    // Set the different components of the Compression Info object,
    // or the entire set of bytes at once
    // Parameters:
    //   ccd_id       - Sets the internal parameter ccd_id
    //   col - Sets the internal parameter col
    //   row  - Sets the internal parameter row
    //   compr_info - Sets the compount parameter (entire value)
    //----------------------------------------------------------------------
    void set(uint16_t accd_id, uint16_t arow, uint16_t acol,
             uint32_t thedata = 0xffffffff)
    {
        if (thedata == 0xffffffff) {
            ccd_id = accd_id;
            col = acol;
            row = arow;
            data.value = (((ccd_id & 0x0000007f) << 26) |
                          ((col & 0x00001fff) << 13) |
                          (row & 0x00001fff));
        } else {
            data.value = thedata;
            ccd_id = (thedata >> 26) & 0x0000007f;
            col = (thedata >> 13) & 0x00001fff;
            row = thedata & 0x00001fff;
        }
    }

    //----------------------------------------------------------------------
    // Method: read
    // Takes the content of the object from the binary stream
    //----------------------------------------------------------------------
    bool read(ifstream & fh)
    {
        data.read(fh);
        set(0, 0, 0, data.value);
        return true;
    }

    //----------------------------------------------------------------------
    // Method: info
    // String with the representation of the object
    //----------------------------------------------------------------------
    string info()
    {
        stringstream s("");
        s << "CCD-" << ccd_id << '_' << row << '-' << col;
        return s.str();
    }
};

//}

#endif // VIS_BASE_H
