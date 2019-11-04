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
 * Copyright (C) 2015-2019 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement sig.fault handler and backtrace functions
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

#include "bt.h"

#ifdef DEFINE_SEGFAULT_BACKTRACE

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//------------------------------------------------------------

#define BT_BUFF_SIZE 200

void showBacktrace(void)
{
    int j, nptrs;
    void *buffer[BT_BUFF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUFF_SIZE);
    fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++) {
        fprintf(stderr, "%s\n", strings[j]);
    }
    
    free(strings);
}

void segfault_handler(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    exit(0);
}

void install_segfault_handler(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_handler;
    sa.sa_flags   = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);
}

#else

void showBacktrace(void) {}
void install_segfault_handler(void) {}

#endif // DEFINE_SEGFAULT_BACKTRACE
