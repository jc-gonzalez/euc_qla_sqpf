/******************************************************************************
 * File:    prodloc.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.ProductLocator
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
 *   Implement ProductLocator class
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

#include "prodloc.h"
#include "rwc.h"
#include "voshdl.h"
#include "filetools.h"

#include <unistd.h>
#include <cassert>
#include <ctime>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <libgen.h>

#define TRACES
#undef  TRACES

#ifdef TRACES
#  define TRC(s)  std::cerr << s << '\n'
#else
#  define TRC(s)
#endif

std::string ProductLocator::master_address;
std::string ProductLocator::remote_address;
bool ProductLocator::isRemote = false;

//----------------------------------------------------------------------
// Method: toLocalArchive
//----------------------------------------------------------------------
bool ProductLocator::toLocalArchive(ProductMeta & m, WorkArea & wa,
                                    ProductLocatorMethod method)
{
    //std::string origPath = m["fileinfo"]["path"];
    std::string origBaseName = m["fileinfo"]["base"];
    std::string origFile = m["fileinfo"]["full"];
    std::string newFile = wa.archive + "/" + origBaseName;
    bool result = relocate(origFile, newFile, method) == 0;
    if (result) {
        m["fileinfo"]["full"] = newFile;
        m["fileinfo"]["path"] = wa.archive;
    }
    return result;
}

//----------------------------------------------------------------------
// Method: toTaskInput
//----------------------------------------------------------------------
bool ProductLocator::toTaskInput(ProductMeta & m, WorkArea & wa,
                                 std::string & taskId,
                                 ProductLocatorMethod method)
{
    std::string origBaseName = m["fileinfo"]["base"];
    std::string origFile = m["fileinfo"]["full"];
    std::string newPath = wa.tasks + "/" + taskId + "/in";
    std::string newFile = newPath + "/" + origBaseName;
    bool result = relocate(origFile, newFile, method) == 0;
    if (result) {
        m["fileinfo"]["full"] = newFile;
        m["fileinfo"]["path"] = newPath;
    }
    return result;
}
 
//----------------------------------------------------------------------
// Method: toLocalOutputs
//----------------------------------------------------------------------
bool ProductLocator::toLocalOutputs(ProductMeta & m, WorkArea & wa,
                                    ProductLocatorMethod method)
{
    std::string origBaseName = m["fileinfo"]["base"];
    std::string origFile = m["fileinfo"]["full"];
    std::string newFile = wa.localOutputs + "/" + origBaseName;
    bool result = relocate(origFile, newFile, method) == 0;
    if (result) {
        m["fileinfo"]["full"] = newFile;
        m["fileinfo"]["path"] = wa.localOutputs;
    }
    return result;
}
 
//----------------------------------------------------------------------
// Method: toLocalInbox
//----------------------------------------------------------------------
bool ProductLocator::toLocalInbox(ProductMeta & m, WorkArea & wa,
                                  ProductLocatorMethod method)
{
    std::string origBaseName = m["fileinfo"]["base"];
    std::string origFile = m["fileinfo"]["full"];
    std::string newFile = wa.localInbox + "/" + origBaseName;
    bool result = relocate(origFile, newFile, method) == 0;
    if (result) {
        m["fileinfo"]["full"] = newFile;
        m["fileinfo"]["path"] = wa.localInbox;
    }
    return result;
}
 
//----------------------------------------------------------------------
// Method: sendToVOSpace
//----------------------------------------------------------------------
bool ProductLocator::sendToVOSpace(std::string user, std::string pwd,
                                   std::string vosURL, std::string folder,
                                   std::string oFile)
{
    VOSpaceHandler vos(new RWC, vosURL);
    vos.setAuth(user, pwd);
    if (!vos.uploadFile(folder, oFile)) {
        //TRC("ERROR! Cannot upload " << oFile);
        return false;
    }
    return true;
}

//----------------------------------------------------------------------
// Method: relocate
//----------------------------------------------------------------------
int ProductLocator::relocate(std::string & sFrom, std::string & sTo,
                             ProductLocatorMethod method, int msTimeOut)
{
    // Wait for the file to appear
    if (msTimeOut != 0) {
        // If timeout is > 0, wait unti timeout is completed
        if (msTimeOut < 0) {
            // If timeout < 0, wait forever until stat is successful
            // (in fact, set a very large value for timeout, say 1 minute)
            msTimeOut = 3000;
        }
        struct stat buffer;
        struct timespec tsp1, tsp2;
        long elapsed = 0;
        (void)clock_gettime(CLOCK_REALTIME_COARSE, &tsp1);
        while ((stat(sFrom.c_str(), &buffer) != 0) || (elapsed > msTimeOut)) {
            (void)clock_gettime(CLOCK_REALTIME_COARSE, &tsp2);
            elapsed = ((tsp2.tv_sec - tsp1.tv_sec) * 1000 +
                       (tsp2.tv_nsec - tsp1.tv_nsec) / 1000000);
        }
        if (elapsed > msTimeOut) {
            //TRC("ERROR: Timeout of " + std::to_string(msTimeOut) +
            //              "ms before successful stat:\t" +
            //              sFrom + std::string(" => ") + sTo);
            return -1;
        }
    }

    int retVal = 0;
    switch(method) {
    case LINK:
        //(void)unlink(sTo.c_str());
        retVal = link(sFrom.c_str(), sTo.c_str());
        TRC("LINK: Hard link of " << sFrom << " to " << sTo);
        break;
    case SYMLINK:
        //(void)unlink(sTo.c_str());
        retVal = symlink(sFrom.c_str(), sTo.c_str());
        TRC("SYMLINK: Soft link of " << sFrom << " to " << sTo);
        break;
    case MOVE:
        retVal = rename(sFrom.c_str(), sTo.c_str());
        TRC("MOVE: Moving file from " << sFrom << " to " << sTo
            << "   retVal=" << retVal);
        if (retVal != 0) {
            TRC("MOVE: errno=" << errno << "  (EXDEV:" << EXDEV
                << ",EEXIST:" << EEXIST << ")");
            if (errno = EXDEV) {
                // Error due to move between different logical devices
                // Try copy & remove
                if ((retVal = FileTools::copyfile(sFrom, sTo)) == 0) {
                    (void)unlink(sFrom.c_str());
                }
            } else if (errno = EEXIST) {
                // File with same name is already at target location
                // Simply remove src.
                (void)unlink(sFrom.c_str());
            }
        } else {
            struct stat buffer;
            if (stat(sFrom.c_str(), &buffer) == 0) {
                (void)unlink(sFrom.c_str());
            }
        }
        break;
    case COPY:
        retVal = FileTools::copyfile(sFrom, sTo);
        TRC("COPY: Copying file from " << sFrom << " to " << sTo);
        break;
    case COPY_TO_REMOTE:
    case COPY_TO_MASTER:
        retVal = FileTools::rcopyfile(sFrom, sTo, master_address, method == COPY_TO_REMOTE);
        TRC(((method == COPY_TO_REMOTE) ? "COPY_TO_REMOTE: " : "COPY_TO_MASTER: ")
            << "Transferring file from " << sFrom << " to " << sTo);
        break;
    default:
        break;
    }

    if (retVal != 0) {
        perror(("ERROR (" + std::to_string(retVal) + "/" + std::to_string(errno) +
                ") relocating product:\n\t" +
                sFrom + std::string(" => ") + sTo).c_str());
        abort();
        //showBacktrace();
    }
    return retVal;
}

//----------------------------------------------------------------------
// Method: setRemote
//----------------------------------------------------------------------
void ProductLocator::setRemote(bool rmte)
{
    isRemote = rmte;
}

//----------------------------------------------------------------------
// Method: setRemoteCopyParams
//----------------------------------------------------------------------
void ProductLocator::setRemoteCopyParams(std::string maddr, std::string raddr)
{
    master_address = maddr;
    remote_address = raddr;
    isRemote = true;
    //TRC("Master addr: " << maddr << "  Remote addr: " << raddr);
}
