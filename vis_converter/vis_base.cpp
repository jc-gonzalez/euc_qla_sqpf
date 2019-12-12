/******************************************************************************
 * File:    vis_base.cpp
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
 *   Implement VIS_Base class
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

#include "vis_base.h"

//namespace VIS {

//----------------------------------------------------------------------
// Class: VISSize
//----------------------------------------------------------------------
const int VISSize::ROWS = 4172;
const int VISSize::COLS = 4256;
const int VISSize::ROWS_HALF = ROWS / 2;
const int VISSize::COLS_HALF = COLS / 2;

const int VISSize::ROWS_FPA = ROWS;
const int VISSize::COLS_FPA = COLS;
const int VISSize::ROWS_QUAD = ROWS_HALF;
const int VISSize::COLS_QUAD = COLS_HALF;

const int VISSize::QUAD_PRE_SCAN_COLS = 51;
const int VISSize::QUAD_OVER_SCAN_COLS = 20;

const int VISSize::CHARGE_INJEC_STRUCT_ROWS = 20;
const int VISSize::QUAD_CHARGE_INJEC_STRUCT_ROWS = CHARGE_INJEC_STRUCT_ROWS / 2;

// Sequences
const vector<string> SequenceIdName = {"ONLY READOUT",
                                       "SIMULATED SIENCE",
                                       "BIAS",
                                       "BIAS With LIMITED SCAN",
                                       "CHARGE INJECTION",
                                       "DARK",
                                       "DARK With LIMITED SCAN",
                                       "FLATFIELD",
                                       "LINEARITY`",
                                       "NOMINAL/SHORT",
                                       "NOMINAL/SHORT with LIMITED SCAN",
                                       "NOMINAL with CHARGE INJECTION",
                                       "SERIAL TRAP PUMPING",
                                       "SERIAL TRAP PUMPING with MULTIPLE READING",
                                       "VERTICAL TRAP PUMPING"};

uint32_t MSK32 = 0xffffffff;
uint32_t MSK16 = 0xffff;
uint32_t MSK8 = 0xff;

/*
 * Compression/decompression :
 *  Raw mode
 *   options_mask |= SZ_RAW_OPTION_MASK;
 *  Nearest neighbor compression mode
 *   options_mask |= SZ_NN_OPTION_MASK;
 *  MSB
 *   options_mask |= SZ_MSB_OPTION_MASK;
 *  ???
 *   options_mask |= SZ_ALLOW_K13_OPTION_MASK;
 */
unsigned int szipOptMask = (SZ_RAW_OPTION_MASK |
                            SZ_NN_OPTION_MASK |
                            SZ_MSB_OPTION_MASK |
                            SZ_ALLOW_K13_OPTION_MASK);

unsigned int bitsPerPixel = 16;
unsigned int bytesPerPixel = bitsPerPixel / 8;

// Input and Output Types

#define T(a, b)  #a
vector<string> InputTypeTag = { T_INPUT_TYPE };
#undef T
#define T(a, b)  {#a, b}
map<string, string> InputTypeDesc = { T_INPUT_TYPE };
#undef T
#define T(a, b)  {#a, IN_ ## a}
map<string, InputType> InputTypeId = { T_INPUT_TYPE };
#undef T

#define T(a, b)  #a
vector<string> OutputTypeTag = { T_OUTPUT_TYPE };
#undef T
#define T(a, b)  {#a, b}
map<string, string> OutputTypeDesc = { T_OUTPUT_TYPE };
#undef T
#define T(a, b)  {#a, OUT_ ## a}
map<string, OutputType> OutputTypeId = { T_OUTPUT_TYPE };
#undef T

//}
