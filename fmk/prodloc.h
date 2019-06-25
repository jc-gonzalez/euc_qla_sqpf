/******************************************************************************
 * File:    Prodloc.h
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
 *   Declare ProductLocator class
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

#ifndef PRODUCTLOCATOR_H
#define PRODUCTLOCATOR_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   - iostream
//------------------------------------------------------------
#include <iostream>

//------------------------------------------------------------
// Topic: External packages
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//------------------------------------------------------------
#include "types.h"
#include "wa.h"

//==========================================================================
// Class: ProductLocator
//==========================================================================
class ProductLocator {

public:
    //----------------------------------------------------------------------
    // Destructor
    //----------------------------------------------------------------------
    virtual ~ProductLocator() {}

    enum ProductLocatorMethod {
        LINK,
        MOVE,
        COPY,
        COPY_TO_REMOTE,
        COPY_TO_MASTER,
        SYMLINK
    };

    //----------------------------------------------------------------------
    // Method: toLocalArchive
    //----------------------------------------------------------------------
    static bool toLocalArchive(ProductMeta & m, WorkArea & wa,
			       ProductLocatorMethod method = LINK);
 
    //----------------------------------------------------------------------
    // Method: toTaskInput
    //----------------------------------------------------------------------
    static bool toTaskInput(ProductMeta & m, WorkArea & wa,
			    std::string & taskId,
			    ProductLocatorMethod method = LINK);
 
    //----------------------------------------------------------------------
    // Method: toLocalOutputs
    //----------------------------------------------------------------------
    static bool toLocalOutputs(ProductMeta & m, WorkArea & wa,
			       ProductLocatorMethod method = MOVE);
 
    //----------------------------------------------------------------------
    // Method: toLocalInbox
    //----------------------------------------------------------------------
    static bool toLocalInbox(ProductMeta & m, WorkArea & wa,
			     ProductLocatorMethod method = MOVE);
 
    //----------------------------------------------------------------------
    // Method: sendToVOSpace
    //----------------------------------------------------------------------
    static bool sendToVOSpace(std::string user, std::string pwd,
			      std::string vosURL, std::string folder,
			      std::string oFile);
    
    //----------------------------------------------------------------------
    // Method: relocate
    //----------------------------------------------------------------------
    static int relocate(std::string & sFrom, std::string & sTo,
			ProductLocatorMethod method = LINK, int msTimeOut = 0);

    //----------------------------------------------------------------------
    // Method: setRemote
    //----------------------------------------------------------------------
    static void setRemote(bool rmte);

    //----------------------------------------------------------------------
    // Method: setRemoteCopyParams
    //----------------------------------------------------------------------
    static void setRemoteCopyParams(std::string maddr, std::string raddr);

private:
    //----------------------------------------------------------------------
    // Constructor
    //----------------------------------------------------------------------
    ProductLocator() {}


private:
    static std::string master_address;
    static std::string remote_address;
    static bool isRemote;
};

#endif // PRODUCTLOCATOR_H
