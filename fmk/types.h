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
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <map>

#include <algorithm>
#include <stdexcept>
#include <memory>

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>

#include "q.h"

using std::queue;
using std::string;
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
#include "json.hpp"
using json = nlohmann::json;

template <class T> class Queue;

typedef const char *   cstr;

typedef json           dict;
typedef json           Config;

typedef std::string    ProductName;
typedef json           ProductMeta; 

typedef Queue<string>  ProductList;
typedef vector<ProductMeta> ProductMetaList;

typedef json           TaskInfo;

typedef int            percent;

typedef map<string, int> CntrSpectrum;

class DirWatcher;
typedef std::tuple<DirWatcher *, Queue<string> &> DirWatchedAndQueue;

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

//== AgentsInfo

typedef map<string, int> AgentSpectrum;

string agentSpectrumToStr(AgentSpectrum & sp);

struct AgentData {
    int num_tasks;
    string task_id;
    string cont_id;
    TaskStatus cont_status;
    AgentSpectrum spectrum;

    string str() {
        return ("\"num_tasks\": " + std::to_string(num_tasks) + ", " +
                "\"task_id\": \"" + task_id + "\", " +
                "\"cont_id\": \"" + cont_id + "\", " +
                "\"cont_status\": \"" + TaskStatusStr[cont_status] + "\", " +
                "\"spectrum\": " + agentSpectrumToStr(spectrum) + "}");
    }
};

typedef map<string, AgentData> AgentsData;
typedef vector<string> AgentNames;
typedef vector<int> AgentTasks;

struct AgentsInfo {
    AgentsData agents;
    AgentNames agent_names;
    AgentTasks agent_num_tasks;

    string str() {
        string s1("");
        string s2("");
        string s3("");
        int n = agent_names.size();
        for (int i = 0; i < n; ++i) {
            string comma((i < (n-1)) ? ", " : "");
            string const & agName = agent_names.at(i);
            auto const & agit = agents.find(agName);
            s1 += "\"" + agName + "\": " + agit->second.str() + comma;
            s2 += "\"" + agName + "\"" + comma;
            s3 += std::to_string(agent_num_tasks.at(i)) + comma;
        }
        return ("{\"agents\": {" + s1 + "}, " +
                "\"agent_names\": [" + s2 + "], " +
                "\"agent_num_tasks\": [" + s3 + "]}");                
    }
};

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
