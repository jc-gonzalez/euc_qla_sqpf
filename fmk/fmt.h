/******************************************************************************
 * File:    q.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.fmt
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
 *   Declare handy fmt() function
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

#ifndef FMT_H
#define FMT_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <string>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------

std::string fmt(const char *format) {
    return format ? format : "";
}

template<typename Type, typename... Args>
std::string fmt( const char *format, const Type& value, Args... args)
{
    std::stringstream strm;
    if ( format ) {
        do {
            if ('$' == *format) {
                strm << value;
                strm << fmt(format+1, args...);
                return strm.str();
            }
            strm << *format++;
        } while (*format);
    }
    //assert(!"too many args");
    return strm.str();
}

// usage: std::cout << fmt("Hello, $, this is the $-th test.", "JC", 12) << std::endl;

#endif // FMT_H
