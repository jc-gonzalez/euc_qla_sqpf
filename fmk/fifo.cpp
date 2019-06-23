/******************************************************************************
 * File:    fifo.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.FiFo
 *
 * Last update:  1.0
 *
 * Date:    20190614
 *
 * Author:  J C Gonzalez
 *
 * Copyright (C) 2019 Euclid SOC Team / J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement FiFo class
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

#include "fifo.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
template<typename T>
FiFo<T>::FiFo(int sz)
{
    init();
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
template<typename T>
FiFo<T>::~FiFo()
{
}

//----------------------------------------------------------------------
// Method: init
// Initialize the component
//----------------------------------------------------------------------
template<typename T>
void FiFo<T>::init()
{
}

