/******************************************************************************
 * File:    expo_duration.h
 *          This file is part of Tools
 *
 * Domain:  tools.ExposureDuration
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
 *   Declare ExposureDuration class
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

#ifndef EXPOSUREDURATION_H
#define EXPOSUREDURATION_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------

//==========================================================================
// Class: ExposureDuration
//==========================================================================
class ExposureDuration {

public:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ExposureDuration();

    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~ExposureDuration();

protected:

private:
    //----------------------------------------------------------------------
    // Method: init
    //----------------------------------------------------------------------
    virtual void init();

private:
    int id;
};

#endif // EXPOSUREDURATION_H
