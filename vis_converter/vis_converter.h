/******************************************************************************
 * File:    vis_converter.h
 *          This file is part of Tools
 *
 * Domain:  tools.VIS_Converter
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
 *   Declare VIS_Converter class
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

#ifndef VIS_CONVERTER_H
#define VIS_CONVERTER_H

//======================================================================
// Group: External Dependencies
//======================================================================

//----------------------------------------------------------------------
// Topic: System headers
//----------------------------------------------------------------------
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

//----------------------------------------------------------------------
// Topic: External packages
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Topic: Project headers
//----------------------------------------------------------------------
#include "raw_vis_hdr.h"
#include "raw_vis_scipckt.h"
#include "le1_vis_ccd.h"

//namespace VIS {

//==========================================================================
// Class: VIS_Converter
//==========================================================================
class VIS_Converter {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    VIS_Converter();

    //----------------------------------------------------------------------
    // Function: run
    // Launches the entire retrieving and generation process
    //----------------------------------------------------------------------
    bool run(VisArgs & someArgs);

private:
    //----------------------------------------------------------------------
    // Function: fileSize
    // Returns the file size in bytes
    //----------------------------------------------------------------------
    long fileSize(std::string filename, std::ifstream::pos_type & token);

    //----------------------------------------------------------------------
    // Function: run
    // Launches the entire retrieving and generation process
    //----------------------------------------------------------------------
    bool checkArgs(VisArgs & someArgs);

    //----------------------------------------------------------------------
    // Function: ccdCoord2ImgExtNumber
    // Computes the image extension number from the quadrant coordinates
    //----------------------------------------------------------------------
    int ccdCoord2ImgExtNumber(int row, int col, int quad);

    //----------------------------------------------------------------------
    // Function: createFits
    // Create a FITS file with the content of the input file as specified
    //----------------------------------------------------------------------
    bool createFits();

    //----------------------------------------------------------------------
    // Function: createCcdFits
    // Create a FITS file with the content of entire CCDs as image extensions
    //----------------------------------------------------------------------
    bool createCcdFits();

    //----------------------------------------------------------------------
    // Function: createLe1Fits
    // Create a FITS file with the LE1 format (1 quad per image extension)
    //----------------------------------------------------------------------
    bool createLe1Fits();

    //----------------------------------------------------------------------
    // Function: createFpaFits
    // Create a FITS file with the content of the entire FPA as one image
    //----------------------------------------------------------------------
    bool createFpaFits();

public:
    VisArgs args;

    string inputFileName;
    string outputFileName;
    InputType inputType;
    OutputType outputType;

    RAW_VIS_Header_base * phdr;
    RAW_VIS_SciPacket * psciPkt;

    vector<LE1_VIS_CCD> ccds;
    LE1_VIS_CCD ccd;
    int ccdIdx;

    struct CCDCoord {
        CCDCoord(int r, int c, int q, int n, int i)
            : row(r), col(c), quad(q), nccd(n), imgIdx(i) {}
        int row; // CCD row in FPA
        int col; // CCD col in FPA
        int quad; // Quadrant (1:E, 2:F, 3:G, 4:H)
        int nccd; // Index of CCD as read from raw input
        int imgIdx; // Image extension index in FITS (1 ext. per quadrant)
    };

    map<int, CCDCoord> imgExt2CcdCoord;
};

//}

#endif // VIS_CONVERTER_H
