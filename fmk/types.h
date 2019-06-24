/******************************************************************************
 * File:    types.h
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.types
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
 *   Declare BalancingMode several handy classes and functions
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
#include <string>
#include <queue>
#include <vector>
#include <string>
#include <map>

#include <algorithm>
#include <stdexcept>

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>

#include "q.h"

using std::queue;
using std::string;
using std::vector;
using std::map;

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "json.h"
template <class T> class Queue;

typedef const char *   cstr;

typedef json::Value    js;
typedef json::Array    jsa;
typedef json::Object   jso;

typedef json::Object   dict;

typedef dict           Config;

typedef std::string    ProductName;
typedef dict           ProductMeta; 

typedef Queue<string>  ProductList;
typedef vector<ProductMeta> ProductMetaList;

typedef json::Value    TaskInfo;

#define forever for(;;)

extern mode_t PathMode;


//==========================================================================
// Enum: TaskStatus
//==========================================================================
#define TTASK_STATUS_LIST \
    T(SCHEDULED,    -2), \
    T(FAILED,       -1), \
    T(FINISHED,      0), \
    T(RUNNING,       1), \
    T(PAUSED,        2), \
    T(STOPPED,       3), \
    T(ABORTED,       4), \
    T(ARCHIVED,      5), \
    T(UNKNOWN_STATE, 6)

#define T(a, b) TASK_ ## a = b
enum TaskStatusEnum { TTASK_STATUS_LIST };
#undef T

extern map<int, string> TaskStatusStr;
extern map<string, int> TaskStatusVal;

class TaskStatus {
private:
    TaskStatusEnum value;
    TaskStatus() {}

public:
    TaskStatus(const TaskStatusEnum& v) : value{v} {} //not explicit here.
 TaskStatus(const int i) : value{TaskStatusEnum(i)} {} //not explicit here.
    operator TaskStatusEnum() const { return value; }
    TaskStatus& operator=(TaskStatusEnum v) { value = v; return *this;}
    bool operator==(const TaskStatusEnum v) const { return value == v; }
    bool operator!=(const TaskStatusEnum v) const { return value != v; }
    operator std::string() const;
    operator int() const;
    std::string str() const;
    void fromStr(std::string & s);
};

//==========================================================================
// Enum: BalancingMode
//==========================================================================
#define TBALANCING_MODE_LIST \
    T(Sequential,    0), \
    T(LoadBalance,   1), \
    T(Random,        2)

#define T(a, b) BALANCE_ ## a = b
enum BalancingModeEnum { TBALANCING_MODE_LIST };
#undef T

extern map<int, string> BalancingModeStr;
extern map<string, int> BalancingModeVal;

class BalancingMode {
private:
    BalancingModeEnum value;
    BalancingMode() {}

public:
    BalancingMode(const BalancingModeEnum& v) : value{v} {} //not explicit here.
    operator BalancingModeEnum() const { return value; }
    BalancingMode& operator=(BalancingModeEnum v) { value = v; return *this;}
    bool operator==(const BalancingModeEnum v) const { return value == v; }
    bool operator!=(const BalancingModeEnum v) const { return value != v; }
    operator std::string() const;
    operator int() const;
    std::string str() const;
    void fromStr(std::string & s);
};

struct TskStatSpectra {
    TskStatSpectra(int r, int s, int p, int st, int fl, int f) :
        running(r), scheduled(s), paused(p),
        stopped(st), failed(fl), finished(f),
        total(r+s+p+st+fl+f) {}
    int    running;
    int    scheduled;
    int    paused;
    int    stopped;
    int    failed;
    int    finished;
    int    total;
};

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

//== UNUSED ================================
#define UNUSED(x) (void)(x)

#endif // TYPES_H
