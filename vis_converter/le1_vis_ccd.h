/******************************************************************************
 * File:    le1_vis_ccd.h
 *          This file is part of Tools
 *
 * Domain:  tools.LE1_VIS_CCD
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

#ifndef LE1_VIS_CCD_H
#define LE1_VIS_CCD_H

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
// Class: LE1_VIS_CCD
//==========================================================================
class LE1_VIS_CCD {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    LE1_VIS_CCD();

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    string info();

    //----------------------------------------------------------------------
    // Method: reset
    //----------------------------------------------------------------------
    void reset();

    //----------------------------------------------------------------------
    // Method: info
    //----------------------------------------------------------------------
    void set(int n) { ccd = n; }

    //----------------------------------------------------------------------
    // Method: storeDataAtPos
    // Copy the data from the input array into the specified row of the CCD
    //----------------------------------------------------------------------
    bool storeDataAtPos(uint8_t * d, int row, int col,
                        bool reorder);

private:
    //----------------------------------------------------------------------
    // Method: defineQuadrant
    // Set the appropriate object flags according to the position
    //----------------------------------------------------------------------
    void defineQuadrant(int row, int col);

    //----------------------------------------------------------------------
    // Method: dataCopy
    // Copy the data from the input array into the specified row of the CCD
    // flipping the data if neccesary, but without reordering
    //----------------------------------------------------------------------
    bool dataCopy(uint8_t * d, int row, int col, int npix,
                  bool reversed);

    //----------------------------------------------------------------------
    // Method: dataCopyReordered
    // Copy the data from the input array into the specified row of the CCD
    // flipping the data if neccesary, and WITH reordering
    //----------------------------------------------------------------------
    bool dataCopyReordered(uint8_t * d, int row, int col, int npix,
                           int regions);


public:
    int ccd;
    uint8_t * raw;
    uint16_t * linealImg;
    uint16_t ** img;

    bool isQuadE, isQuadF, isQuadG, isQuadH;
    bool isQuadEorF, isQuadGorH, isQuadEorH, isQuadForG;
    int quadIdx;
    string quad;

};

//}

#endif // LE1_VIS_CCD_H
