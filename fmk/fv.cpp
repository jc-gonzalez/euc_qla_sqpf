/******************************************************************************
 * File:    fv.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.FileVersion
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
 *   Implement FileVersion class
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

#include "fv.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
FileVersion::FileVersion()
    : mj(0), mn(0)
{}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
FileVersion::FileVersion(unsigned int ma, unsigned int mi)
    : mj(ma), mn(mi)
{}

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
FileVersion::FileVersion(std::string v)
{
    setVersion(v);
}

//----------------------------------------------------------------------
// Method: major
// Get major version number
//----------------------------------------------------------------------
unsigned int FileVersion::major()
{
    return mj;
}

//----------------------------------------------------------------------
// Method: minor
// Get minor version number
//----------------------------------------------------------------------
unsigned int FileVersion::minor()
{
    return mn;
}

//----------------------------------------------------------------------
// Method: setVersion
// Set version major and minor numbers
//----------------------------------------------------------------------
void FileVersion::setVersion(unsigned int ma, unsigned int mi)
{
    mj = ma;
    mn = mi;
}

//----------------------------------------------------------------------
// Method: serVersion
// Set version from version string
//----------------------------------------------------------------------
void FileVersion::setVersion(std::string s)
{
    std::stringstream ss(s);
    char c;
    ss >> mj >> c >> mn;
}

//----------------------------------------------------------------------
// Method: getVersion
// Get major and minor version components
//----------------------------------------------------------------------
void FileVersion::getVersion(unsigned int & ma, unsigned int & mi)
{
    ma = mj;
    mi = mn;
}

//----------------------------------------------------------------------
// Method: getVersion
// Get version as a string (MJ.mn)
//----------------------------------------------------------------------
std::string FileVersion::getVersion()
{
    char v[6];
    sprintf(v, "%02d.%02d", mj, mn);
    return std::string(v);
}

//----------------------------------------------------------------------
// Method: incr
// Increment version number
//----------------------------------------------------------------------
void FileVersion::incr()
{
    incrMinor();
}

//----------------------------------------------------------------------
// Method: incrMajor
// Increments major version number (minor is set to 0)
//----------------------------------------------------------------------
void FileVersion::incrMajor()
{
    mj++;
    mn = 0;
}

//----------------------------------------------------------------------
// Method: incrMinor
// Increments minor version number (and major, if needed)
//----------------------------------------------------------------------
void FileVersion::incrMinor()
{
    mn++;
    if (mn > 99) { mn = 0, mj++; }
}

