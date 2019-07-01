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
#include <chrono>

template<typename T>
const T FiFo<T>::null = T(); 

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
template<typename T>
FiFo<T>::FiFo(size_t _sz)
    : objects(), queueMutex(), cv(), sz(_sz), ln(0)
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
template<typename T>
FiFo<T>::~FiFo()
{
}

//----------------------------------------------------------------------
// Method: setSize
//----------------------------------------------------------------------
template<typename T>
void FiFo<T>::setSize(size_t _sz)
{
    sz = _sz;
}

//----------------------------------------------------------------------
// Method: put
//----------------------------------------------------------------------
template<typename T>
T FiFo<T>::put(T && obj)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    if (ln == sz) {
        T first = std::move(objects.front());
        objects.pop(); objects.push(std::move(obj));
        return first;
    } else {
        objects.push(std::move(obj));
        ++ln;
        return null;
    }
}

//----------------------------------------------------------------------
// Method: find
//----------------------------------------------------------------------
template<typename T>
bool FiFo<T>::find(T obj)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    bool isIn = false;
    for (int i = 0; i < ln; ++i) {
        T elem = std::move(objects.front());
        isIn |= (elem == obj);
        objects.pop(); objects.push(elem);
    }
    return isIn;
}

//----------------------------------------------------------------------
// Method: empty
//----------------------------------------------------------------------
template<typename T>
bool FiFo<T>::empty()
{
    bool isEmpty;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isEmpty = objects.empty();
    }
    return isEmpty;
}

//----------------------------------------------------------------------
// Method: size
//----------------------------------------------------------------------
template<typename T>
size_t FiFo<T>::size()
{
    size_t _sz;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        _sz = sz;
    }
    return _sz;
}

//----------------------------------------------------------------------
// Method: len
//----------------------------------------------------------------------
template<typename T>
size_t FiFo<T>::len()
{
    size_t _ln;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        _ln = ln;
    }
    return _ln;
}

template class FiFo<std::string>;
