/******************************************************************************
 * File:    types.cpp
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
#include "str.h"

mode_t PathMode = 0755;

//==========================================================================
// Enum: TaskStatus
//==========================================================================

#define T(a, b) {b, string(#a)}
map<int, string> TaskStatusStr = { TTASK_STATUS_LIST };
#undef T

#define T(a, b) {string(#a), b}
map<string, int> TaskStatusVal = { TTASK_STATUS_LIST };
#undef T

TaskStatus::operator std::string() const
{
    return TaskStatusStr[value];
}

TaskStatus::operator int() const
{
    return int(value);
}

std::string TaskStatus::str() const
{
    return TaskStatusStr[value];
}

void TaskStatus::fromStr(std::string & s)
{
    value = TaskStatusEnum(TaskStatusVal[s]);
}

//==========================================================================
// Enum: BalancingMode
//==========================================================================

#define T(a, b) {b, string(#a)}
map<int, string> BalancingModeStr = { TBALANCING_MODE_LIST };
#undef T

#define T(a, b) {string(#a), b}
map<string, int> BalancingModeVal = { TBALANCING_MODE_LIST };
#undef T

BalancingMode::operator std::string() const
{
    return BalancingModeStr[value];
}

BalancingMode::operator int() const
{
    return int(value);
}

std::string BalancingMode::str() const
{
    return BalancingModeStr[value];
}

void BalancingMode::fromStr(std::string & s)
{
    value = BalancingModeEnum(BalancingModeVal[s]);
}

string agentSpectrumToStr(AgentSpectrum & sp) {
    std::stringstream ss;
    ss << "{" 
       << "\"ABORTED\": "   << sp["ABORTED"]   << ", " 
       << "\"ARCHIVED\": "  << sp["ARCHIVED"]  << ", " 
       << "\"FAILED\": "    << sp["FAILED"]    << ", " 
       << "\"FINISHED\": "  << sp["FINISHED"]  << ", " 
       << "\"PAUSED\": "    << sp["PAUSED"]    << ", " 
       << "\"RUNNING\": "   << sp["RUNNING"]   << ", " 
       << "\"SCHEDULED\": " << sp["SCHEDULED"] << ", " 
       << "\"STOPPED\": "   << sp["STOPPED"]   << "}";
    return ss.str();
}

//======================================================================
// fmt
//======================================================================

std::string fmt(const char *format) {
    return format ? format : "";
}

// usage: std::cout << fmt("Hello, $, this is the $-th test.", "JC", 12) << std::endl;
