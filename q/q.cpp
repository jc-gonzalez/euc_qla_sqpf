/******************************************************************************
 * File:    q.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.Queue
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
 *   Implement Queue class
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

#include "q.h"
#include <chrono>

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
template<typename T>
Queue<T>::Queue() : objects(), queueMutex(), cv()
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
template<typename T>
Queue<T>::~Queue()
{
}

//----------------------------------------------------------------------
// Method: append
//----------------------------------------------------------------------
template<typename T>
void Queue<T>::append(Queue<T> & other)
{
    T obj;
    while (other.get(obj)) { push(std::move(obj)); }
}

//----------------------------------------------------------------------
// Method: push
//----------------------------------------------------------------------
template<typename T>
void Queue<T>::push(T && obj)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        objects.push(std::move(obj));
    }
    cv.notify_one();
}

//----------------------------------------------------------------------
// Method: get
//----------------------------------------------------------------------
template<typename T>
bool Queue<T>::get(T & obj, int msTimeout)
{
    static const auto ms = std::chrono::milliseconds(1);
    std::unique_lock<std::mutex> lock(queueMutex);
    //std::lock_guard<std::mutex> lock(queueMutex);
    if (!cv.wait_for(lock, msTimeout * ms,
                     [this](){return !objects.empty();})) return false;
    obj = std::move(objects.front());
    objects.pop();
    return true;
}

//----------------------------------------------------------------------
// Method: empty
//----------------------------------------------------------------------
template<typename T>
bool Queue<T>::empty()
{
    bool isEmpty;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isEmpty = objects.empty();
    }
    return isEmpty;
}

//----------------------------------------------------------------------
// Method: dump
//----------------------------------------------------------------------
template<typename T>
void Queue<T>::dump()
{
    queue<T> tmp_q;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tmp_q = objects;
    }
    while (!tmp_q.empty()) {
        T obj = tmp_q.front();
        tmp_q.pop();
    } 
}

template class Queue<std::string>;
