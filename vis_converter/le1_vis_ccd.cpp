/******************************************************************************
 * File:    le1_vis_ccd.cpp
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

#include "le1_vis_ccd.h"

//namespace VIS {

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
LE1_VIS_CCD::LE1_VIS_CCD()
{
    linealImg = new uint16_t [VISSize::ROWS * VISSize::COLS];
    raw = (uint8_t*)linealImg;
    img = new uint16_t * [VISSize::ROWS];
    for (unsigned int i = 0; i < VISSize::ROWS; ++i) {
        img[i] = (uint16_t *)&(linealImg[i * VISSize::COLS]);
    }
}

//----------------------------------------------------------------------
// Method: info
//----------------------------------------------------------------------
string LE1_VIS_CCD::info()
{
    return string("");
}

//----------------------------------------------------------------------
// Method: reset
//----------------------------------------------------------------------
void LE1_VIS_CCD::reset()
{
    memset((void *)linealImg, 0,
           VISSize::ROWS * VISSize::COLS * sizeof(uint16_t));
}

//----------------------------------------------------------------------
// Method: defineQuadrant
// Set the appropriate object flags according to the position
//----------------------------------------------------------------------
void LE1_VIS_CCD::defineQuadrant(int row, int col)
{
    static string lastQuad("");
    static vector<string> quads({"E", "F", "G", "H"});

           isQuadE = isQuadF = isQuadG = isQuadH = false;
    if (row < VISSize::ROWS_HALF) {
        isQuadE = col < VISSize::COLS_HALF;
        isQuadF = ! isQuadE;
    } else {
        isQuadH = col < VISSize::COLS_HALF;
        isQuadG = ! isQuadH;
    }
    isQuadEorF = isQuadE || isQuadF;
    isQuadGorH = isQuadG || isQuadH;
    isQuadEorH = isQuadE || isQuadH;
    isQuadForG = isQuadF || isQuadG;

    quadIdx = (isQuadE ? 0 :
               (isQuadF ? 1 :
                (isQuadG ? 2 : 3)));
    quad = quads[quadIdx];

    if (lastQuad != quad) {
        lastQuad = quad;
        std::cerr << "Quadrant " << quad << '\n';
    }
}

//----------------------------------------------------------------------
// Method: dataCopy
// Copy the data from the input array into the specified row of the CCD
// flipping the data if neccesary, but without reordering
//----------------------------------------------------------------------
bool LE1_VIS_CCD::dataCopy(uint8_t * d, int row, int col, int npix,
                           bool reversed)
{
    int dc = reversed ? -1 : +1;

    for (unsigned int c = col, k = 0, j = 0; k < npix; c += dc, ++k, j += 2) {
        img[row][c] = (((uint16_t)d[j]) << 8) + ((uint16_t)d[j+1]);
    }

    return true;
}

//----------------------------------------------------------------------
// Method: dataCopyReordered
// Copy the data from the input array into the specified row of the CCD
// flipping the data if neccesary, and WITH reordering
//----------------------------------------------------------------------
bool LE1_VIS_CCD::dataCopyReordered(uint8_t * d, int row, int col, int npix,
                                    int regions)
{
    int dr = (isQuadGorH ? -1 : +1);
    int dc = (isQuadForG ? -1 : +1);
    int r = row - dr;
    int c = col - dc;

    //bool thisrow = (((row % 1000) == 0) || ((row > 2084) && (row < 2088)));
    bool thisrow = true;
    if (thisrow) {
        std::cerr << "Storing " << npix << " values at (" << row << ", " << col
                  << ") in  " << regions << " regions:\n";
    }

    uint16_t v;
    for (unsigned int k = 0, j = 0; k < npix; ++k, j += 2) {
        r += dr;
        if ((k % regions) == 0) { r = row, c += dc; }
        v = (((uint16_t)d[j]) << 8) + ((uint16_t)d[j+1]);
        img[r][c] = v;
        if (thisrow) {
            if (((c+1) % 133) == 0) std::cerr << '(' << r << ',' << c << "):"
                                              << v << " .. ";
        }
    }
    if (thisrow) std::cerr << '\n';
    return true;
}

//----------------------------------------------------------------------
// Method: storeDataAtPos
// Copy the data from the input array into the specified row of the CCD
//----------------------------------------------------------------------
bool LE1_VIS_CCD::storeDataAtPos(uint8_t * d, int row, int col,
                                 bool reorder)
{
    defineQuadrant(row, col);

    if (!reorder) {

        if (col < VISSize::COLS_HALF) { // Quadrants E & H
            dataCopy(d, row, col, VISSize::COLS_HALF, false);
        } else { // Quadrants F & G
            dataCopy(d, row, col, VISSize::COLS_HALF, true);
        }

    } else {

        bool is2RowArea = ((row >= (VISSize::ROWS_HALF - 2)) &&
                           (row <= (VISSize::ROWS_HALF + 1)));
        int regions = is2RowArea ? 2 : 4;
        int colblk = VISSize::COLS_HALF / regions;

        dataCopyReordered(d, row, col, VISSize::COLS_HALF, regions);


    }

    return true;
}

//}
