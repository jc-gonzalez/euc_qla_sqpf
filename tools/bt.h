/******************************************************************************
 * File:    bt.h
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libtools.BT
 *
 * Version:  2.0
 *
 * Date:    2019/06/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2018 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare sig.fault handler and backtrace functions
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef BT_H
#define BT_H

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <execinfo.h>

void showBacktrace(void);
void segfault_handler(int signal, siginfo_t *si, void *arg);
void install_segfault_handler(void);

#endif // BT_H
