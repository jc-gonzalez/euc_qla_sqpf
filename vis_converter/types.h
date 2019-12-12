/******************************************************************************
 * File:    types.h
 *          This file is part of Tools
 *
 * Domain:  tools.types
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
 *   Declare basic types
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

#ifndef TYPES_H
#define TYPES_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <map>

#include <algorithm>
#include <stdexcept>
#include <memory>
#include <limits>

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>

using std::ifstream;

using std::queue;
using std::string;
using std::stringstream;
using std::vector;
using std::map;

using std::pair;
using std::make_pair;

using std::shared_ptr;
using std::make_shared;

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "q.h"

//------------------------------------------------------------
// Topic: Predefined constants
//------------------------------------------------------------

extern const uint32_t SpaceWireROEPacketHeaderTag; 
extern const uint32_t SpaceWireROEPacketFooterTag; 

extern const uint32_t SpaceWireSciPacketHeaderTag; 
extern const uint32_t SpaceWireSciPacketFooterTag; 

//------------------------------------------------------------
// Topic: Elementary types and typedefs
//------------------------------------------------------------

#define MAX_TMP_STR 512

template <class T> class Queue;

typedef const char *   cstr;

#define ReverseBytes(...) do {                                          \
        for (size_t byt=0; byt<sizeof(__VA_ARGS__)>>1; ++byt)           \
            ((unsigned char*)&(__VA_ARGS__))[byt] ^=                    \
                ((unsigned char*)&(__VA_ARGS__))[sizeof(__VA_ARGS__)-1-byt], \
                ((unsigned char*)&(__VA_ARGS__))[sizeof(__VA_ARGS__)-1-byt] ^= \
                ((unsigned char*)&(__VA_ARGS__))[byt],                  \
                ((unsigned char*)&(__VA_ARGS__))[byt] ^=                \
                ((unsigned char*)&(__VA_ARGS__))[sizeof(__VA_ARGS__)-1-byt]; \
    } while(0)

template <typename T, int NBYTES>
class UInt {
public:
    uint8_t data[NBYTES];
    T value;
public:
    bool read(ifstream & fh) {
        fh.read((char *)(this->data), NBYTES);
        //std::cerr << fh.gcount() << " bytes\n";
        value = 0;
        for (int i = 0; i < NBYTES; ++i) {
            value |= data[i] << (8 * (NBYTES - 1 - i));
        }
        return true;
    }
    string info() {
        char buffer[MAX_TMP_STR];
        sprintf(buffer, "%0*llX:%s", NBYTES * 2, (unsigned long long)value,
                std::to_string(value).c_str());
        return string(buffer);
    }
};

typedef UInt<uint16_t, 2> UInt16;
typedef UInt<uint32_t, 4> UInt32;
typedef UInt<uint64_t, 6> UInt48;
typedef UInt<uint64_t, 8> UInt64;

#define forever for(;;)


//==========================================================================
// Python style list comprehensions for C++.
// Example: multiply all elements of vector literal by 2.
//
// auto vec_doubled = VFOR(x * 2, x, std::vector<int>({1,2,3}));
// 
// vec_squared is now equal to std::vector<int>({2,4,6})
//==========================================================================

#include <type_traits>

template <typename T, typename Function,
    typename ResultType =
          std::vector<typename std::result_of<Function(const T&)>::type> >
    ResultType vector_comprehension(const std::vector<T>& in_vec,
                                    Function func) {
    ResultType result;
    for (auto& elem : in_vec) { result.push_back(func(elem)); }
    return result;
    }
    
template <typename T>
struct Get: T {};

// Macros apparently don't like C++11 style literals unless wrapped in
// something (in G++ at least).
// So you can't do VFOR(x * x, x, {1,2,3}, ), you have to do 
#define VFOR(expr, var, coll)                                           \
    vector_comprehension<Get<std::remove_reference<decltype(coll)>::type>::value_type> \
    ((coll), [&](Get<std::remove_reference<decltype(coll)>::type>::const_reference var) { return (expr);})

//=== indexOf ==============================

template<typename T>
int indexOf(const std::vector<T>& v, const T& elem)
{
    int npos = std::find(v.begin(), v.end(), elem) - v.begin();
    if (npos >= v.size()) { return -1; } else { return npos; }
}

//======================================================================
// fmt
//======================================================================

std::string fmt(const char *format);

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

//== UNUSED ================================
#define UNUSED(x) (void)(x)

#endif // TYPES_H
