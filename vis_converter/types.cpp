/******************************************************************************
 * File:    types.cpp
 *          This file is part of Tools
 *
 * Domain:  tools.VIS_Base
 *
 * Last update:  1.0
 *
 * Date:    20190614
 *
 * Author:  J C Gonzalez
 *
 * Copyright (C) 2015-2020 Euclid SOC Team / J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement several handy classes
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

#include "types.h"

//------------------------------------------------------------
// Topic: Predefined constants
//------------------------------------------------------------

const uint32_t SpaceWireROEPacketHeaderTag = 0xffffffff; 
const uint32_t SpaceWireROEPacketFooterTag = 0xffffffff; 

const uint32_t SpaceWireSciPacketHeaderTag = 0xeeeeeeee; 
const uint32_t SpaceWireSciPacketFooterTag = 0xffffffff; 


//======================================================================
// fmt
//======================================================================

std::string fmt(const char *format) {
    return format ? format : "";
}

// usage: std::cout << fmt("Hello, $, this is the $-th test.", "JC", 12) << std::endl;
