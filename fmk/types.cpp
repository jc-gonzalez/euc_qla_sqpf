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
