/******************************************************************************
 * File:    vis_converter.cpp
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
 *   Implement VIS_Converter class
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

#include "vis_converter.h"

#include <functional>

extern "C" {
#   include "fitsio.h"
}

//namespace VIS {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
VIS_Converter::VIS_Converter()
{
    int i;
    int n = 0;

    // Compute CCD Coords <-> Img.Ext.number tables
    for (int row = 1; row <= 6; ++row) {
        for (int col = 1; col <= 6; ++col) {
            for (int q = 1; q <= 4; ++q) {
                i = ccdCoord2ImgExtNumber(row, col, q);
                auto newElem = std::pair<int, CCDCoord>(i, CCDCoord(row, col, q, n, i));
                imgExt2CcdCoord.insert(newElem);
            }
            ++n;
        }
    }
    for (auto & kv : imgExt2CcdCoord) {
        auto & cc = kv.second;
        std::cout << cc.row << ", " << cc.col << " - " << cc.quad << " : "
                  << cc.nccd << " - " << cc.imgIdx << '\n';
    }
}

//----------------------------------------------------------------------
// Function: fileSize
// Returns the file size in bytes
//----------------------------------------------------------------------
long VIS_Converter::fileSize(std::string filename, std::ifstream::pos_type & token)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    token = in.tellg();
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

//----------------------------------------------------------------------
// Function: ccdCoord2ImgExtNumber
// Computes the image extension number from the quadrant coordinates
//----------------------------------------------------------------------
int VIS_Converter::ccdCoord2ImgExtNumber(int row, int col, int quad)
{
    return ((row - 1) * 24 + (6 - col) * 4 + quad +
            ((col >= 4) ? 0 : ((quad <= 2) ? +2 : -2)));
}

//----------------------------------------------------------------------
// Function: run
// Launches the entire retrieving and generation process
//----------------------------------------------------------------------
bool VIS_Converter::checkArgs(VisArgs & someArgs)
{
    // Take arguments
    args = someArgs;

    inputFileName = args.iFile;
    outputFileName = args.oFile;

    // Check that input file exists
    struct stat st;
    bool exists = (stat(inputFileName.c_str(), &st) == 0);
    if (! exists) {
        std::cerr << "ERROR: Input file " << inputFileName << " not found!\n";
        return false;
    }

    // Check input file type
    auto iTypeIt = InputTypeId.find(args.iType);
    if (iTypeIt == InputTypeId.end()) {
        std::cerr << "ERROR: Input file type '" << args.iType << "' not supported!\n";
        return false;
    }
    inputType = iTypeIt->second;

    // Check input file type
    auto oTypeIt = OutputTypeId.find(args.oType);
    if (oTypeIt == OutputTypeId.end()) {
        std::cerr << "ERROR: Output file type '" << args.oType << "' not supported!\n";
        return false;
    }
    outputType = oTypeIt->second;

    return true;
}

//----------------------------------------------------------------------
// Function: run
// Launches the entire retrieving and generation process
//----------------------------------------------------------------------
bool VIS_Converter::run(VisArgs & someArgs)
{
    bool retval = checkArgs(someArgs);

    if (!retval) { return false; }

    // Create appropriate input objects
    switch (inputType) {
    case IN_raw:
        phdr = new RAW_VIS_Header;
        break;
    case IN_raw_prev:
        phdr = new RAW_VIS_Header_prev;
        break;
    case IN_raw_old:
        phdr = new RAW_VIS_Header_old;
        break;
    default:
        break;
    }
    RAW_VIS_Header_base & hdr = *phdr;

    RAW_VIS_SciPacket sciPkt;

    SpaceWirePacketTag tag;

    int roePackets = 0;
    int sciPackets = 0;
    int sciPacketsROE = 0;
    vector<int> sciPacketsPerROE;

    ccdIdx = -1;

    bool reorder = true;

    std::ifstream::pos_type endToken;
    long fSize = fileSize(args.iFile.c_str(), endToken);
    ifstream ifHdl(args.iFile, std::ifstream::binary);

    while (ifHdl.tellg() != endToken) {

        tag.read(ifHdl);

        if (tag.is(SpaceWireROEPacketHeaderTag)) {

            tag.rollback(ifHdl);
            hdr.read(ifHdl);
            sciPkt.setCompressionInfo(hdr.comprInfo);
            std::cout << hdr.info() << '\n';
            ++roePackets;
            if (sciPacketsROE > 0) {
                sciPacketsPerROE.push_back(sciPacketsROE);
                sciPacketsROE = 0;
            }

        } else if (tag.is(SpaceWireSciPacketHeaderTag)) {

            tag.rollback(ifHdl);
            sciPkt.read(ifHdl);
            //std::cout << sciPkt.info() << '\n';
            ++sciPackets;
            ++sciPacketsROE;

            if (sciPkt.ccd.ccd_id != ccdIdx) {
                if (ccdIdx != -1) { ccds.push_back(ccd); }
                ccdIdx = sciPkt.ccd.ccd_id;
                std::cerr << "CCD #" << ccdIdx + 1 << '\n';
                ccd.reset();
            }

            ccd.storeDataAtPos((hdr.comprInfo.compr_type == NO_COMPR) ?
                               sciPkt.data : sciPkt.uncomprPixels,
                               sciPkt.ccd.row, sciPkt.ccd.col,
                               hdr.comprInfo.compr_type == WITH_REORDER_121);

        } else {

            std::cerr << "ERROR: Unknown tag: " << tag.value << "\n";
            return false;

        }
    }

    sciPacketsPerROE.push_back(sciPacketsROE);
    ccds.push_back(ccd);

    std::cout << "File " << inputFileName << ": \n"
              << "Size (byes):            " << fSize << " (" << endToken << ")\n"
              << "ROE Header packets:     " << roePackets << '\n'
              << "Science Header packets: " << sciPackets << '\n'
              << "Sci.Packets per ROE:    [ ";
    for (unsigned int i = 0; i < sciPacketsPerROE.size(); ++i) {
        std::cout << sciPacketsPerROE.at(i) << ' ';
    }
    std::cout << "]\n"
              << "CCDs:                   " << ccds.size() << '\n';

    retval = createFits();

    return retval;
}

#include <array>
#include <utility>

template<class T, class... Tail, class Elem = typename std::decay<T>::type>
std::array<Elem,1+sizeof...(Tail)> make_array(T&& head, Tail&&... values)
{
  return { std::forward<T>(head), std::forward<Tail>(values)... };
}

// in code
// auto std_array = make_array(1,2,3,4,5);

//----------------------------------------------------------------------
// Function: createFits
// Create a FITS file with the content of the input file as specified
//----------------------------------------------------------------------
bool VIS_Converter::createFits()
{
    auto fitsCreatorTable =
        make_array(std::mem_fn(&VIS_Converter::createLe1Fits),
                   std::mem_fn(&VIS_Converter::createCcdFits),
                   std::mem_fn(&VIS_Converter::createFpaFits));

    auto fitsCreator = fitsCreatorTable[(int)(outputType)];

    return fitsCreator(*this);
}

//----------------------------------------------------------------------
// Function: createCcdFits
// Create a FITS file with the content of entire CCDs as image extensions
//----------------------------------------------------------------------
bool VIS_Converter::createCcdFits()
{
    //
    // Create FITS file
    //

    std::cerr << "Creating CCD FITS file " << outputFileName << " . . .\n";

    fitsfile *fptr;       /* pointer to the FITS file; defined in fitsio.h */
    int status, ii, jj;
    string sKeywd;
    long fpixel = 1, naxis = 2, nelements, keywd;
    long naxes[2] = {VISSize::COLS, VISSize::ROWS};

    status = 0;         /* initialize status before calling fitsio routines */
    fits_create_file(&fptr, outputFileName.c_str(), &status);   /* create new file */

    nelements = naxes[0] * naxes[1];          /* number of pixels to write */

    for (unsigned int nccd = 0; nccd < ccds.size(); ++nccd) {

        /* Create the primary array image (16-bit short integer pixels */
        fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
        std::cerr << "Creation of image " << nccd << ": "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';

        /* Write a keyword; must pass the ADDRESS of the value */
        sKeywd = "EXPOSURE", keywd = 1500;
        fits_update_key(fptr, TLONG, sKeywd.c_str(), &keywd, "Total Exposure Time", &status);
        std::cerr << "Writing keyword: " << sKeywd << "=" << keywd << " : "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';

        /* Write the array of integers to the image */
        fits_write_img(fptr, TUSHORT, fpixel, nelements, ccds.at(nccd).img[0], &status);
        std::cerr << "Writing image " << nccd << ": "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';
    }

    fits_close_file(fptr, &status);            /* close the file */
    fits_report_error(stderr, status);  /* print out any error messages */

    return true;
}


//----------------------------------------------------------------------
// Function: createLe1Fits
// Create a FITS file with the LE1 format (1 quad per image extension)
//----------------------------------------------------------------------
bool VIS_Converter::createLe1Fits()
{
    //
    // Create FITS file
    //

    std::cerr << "Creating LE1 FITS file " << outputFileName << " . . .\n";

    fitsfile *fptr;       /* pointer to the FITS file; defined in fitsio.h */
    int status, ii, jj, nccd;
    string sKeywd;
    long fpixel = 1, naxis = 2, nelements, keywd;
    long naxes[2] = {VISSize::COLS_QUAD, VISSize::ROWS_QUAD};

    uint16_t * img = new uint16_t [VISSize::ROWS_QUAD * VISSize::COLS_QUAD];
    uint16_t ** qimg = new uint16_t * [VISSize::ROWS_QUAD];
    for (int r = 0; r < VISSize::ROWS_QUAD; ++r) {
        qimg[r] = &(img[r * VISSize::COLS_QUAD]);
    }

    const char * squads = " EFGH";
    string oFile = "!" + outputFileName;

    status = 0;         /* initialize status before calling fitsio routines */
    fits_create_file(&fptr, oFile.c_str(), &status);   /* create new file */

    nelements = naxes[0] * naxes[1];          /* number of pixels to write */

    for (unsigned int imgExt = 1; imgExt <= imgExt2CcdCoord.size(); ++imgExt) {

        auto & cc = imgExt2CcdCoord.find(imgExt)->second;
        nccd = cc.nccd;

        /* Create the primary array image (16-bit short integer pixels */
        fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
        char squad = squads[cc.quad];
        std::cerr << "Creation of image " << imgExt << " (" << cc.nccd << "." << cc.quad << "): "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';

        /* Write a keyword; must pass the ADDRESS of the value */
        sKeywd = "EXPOSURE", keywd = 1500;
        fits_update_key(fptr, TLONG, sKeywd.c_str(), &keywd, "Total Exposure Time", &status);
        std::cerr << "Writing keyword: " << sKeywd << "=" << keywd << " : "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';

        switch (cc.quad) {
        case 1: // E
            for (int i = 0; i < VISSize::ROWS_QUAD; ++i) {
                for (int j = 0; j < VISSize::COLS_QUAD; ++j) {
                    qimg[i][j] = ccds.at(nccd).img[i][j];
                }
            }
            break;
        case 2: // F
            for (int i = 0; i < VISSize::ROWS_QUAD; ++i) {
                for (int j = 0; j < VISSize::COLS_QUAD; ++j) {
                    qimg[i][j] = ccds.at(nccd).img[VISSize::ROWS - 1 - i][j];
                }
            }
            break;
        case 3: // G
            for (int i = 0; i < VISSize::ROWS_QUAD; ++i) {
                for (int j = 0; j < VISSize::COLS_QUAD; ++j) {
                    qimg[i][j] = ccds.at(nccd).img[VISSize::ROWS - 1 - i][VISSize::COLS - 1 - j];
                }
            }
            break;
        case 4: // H
            for (int i = 0; i < VISSize::ROWS_QUAD; ++i) {
                for (int j = 0; j < VISSize::COLS_QUAD; ++j) {
                    qimg[i][j] = ccds.at(nccd).img[i][VISSize::COLS - 1 - j];
                }
            }
            break;
        default:
            continue;
        }

        /* Write the array of integers to the image */
        fits_write_img(fptr, TUSHORT, fpixel, nelements, img, &status);
        std::cerr << "Writing image " << imgExt << ": "
                  << (status == 0 ? "OK" : "FAILED!") << '\n';
    }

    fits_close_file(fptr, &status);            /* close the file */
    fits_report_error(stderr, status);  /* print out any error messages */

    return true;
}


//----------------------------------------------------------------------
// Function: createFpaFits
// Create a FITS file with the content of the entire FPA as one image
//----------------------------------------------------------------------
bool VIS_Converter::createFpaFits()
{
    return true;
}

//}
