/******************************************************************************
 * File:    qdtrephdl.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.qdtrephdl
 *
 * Version:  2.0
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2018 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement QDTReportHandler class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
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

#include "qdtrephdl.h"

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
// namespace QPF {

//----------------------------------------------------------------------
// Method: getIssues
// Retrieves from the data the list of issues found
//----------------------------------------------------------------------
bool QDTReportHandler::getIssues(std::vector<Alert*> & issues)
{
    // Loop on all the products in the report (normally, only 1)
    json::iterator prodIt = data.begin();
    while (prodIt != data.end()) {

        json &  p = prodIt.value();
        std::string product = prodIt.key();
//         //std::cerr << product << '\n';

        // Loop on all the CCDs
        json::iterator ccdIt = p.begin();
        while (ccdIt != p.end()) {

            json &  c = ccdIt.value();
            std::string ccdSet = ccdIt.key();
//             //std::cerr << '\t' << ccdSet << '\n';

            if (ccdSet.compare(0, 3, "CCD") == 0) {
                // Loop on all the quadrant
                json::iterator quadIt = c.begin();
                json::iterator quadItEnd = c.end();
                quadItEnd--;
                while (quadIt != quadItEnd) {

                    json &  q = quadIt.value();
                    std::string quadrant = quadIt.key();
//                     //std::cerr << "\t\t" << quadrant << '\n';

                    // Loop on all the diagnostics for the quadrant
                    json &  qDiagIt = q["diagnostics"];
                    json::iterator diagIt = qDiagIt.begin();
                    while (diagIt != qDiagIt.end()) {

                        std::string diagnostic = diagIt.key();
//                         //std::cerr << "\t\t\t" << diagnostic << '\n';

                        std::string location = (product + "." + ccdSet + "." +
                                                quadrant + "." + diagnostic);

                        checkDiagnostic(diagIt, location, issues);

                        ++diagIt; // next diagnostic
                    }

                    ++quadIt; // next quadrant
                }
            }

            // Loop on all the diagnostics for the entire CCD or Detector
            json &  cDiagIt = c["diagnostics"];
            json::iterator diagIt = cDiagIt.begin();
            while (diagIt != cDiagIt.end()) {

                std::string diagnostic = diagIt.key();
//                 //std::cerr << "\t\t\t" << diagnostic << '\n';

                std::string location = (product + "." + ccdSet + "." +
                                        diagnostic);

                checkDiagnostic(diagIt, location, issues);

                ++diagIt; // next diagnostic
            }

            ++ccdIt; // next CCD
        }

        ++prodIt; // next product
    }

    return true;
}

void QDTReportHandler::checkDiagnostic(json::iterator it,
                                       std::string & location,
                                       std::vector<Alert*> & issues)
{
    Alert::Messages msgs;

    json &  d = it.value();
//     std::cerr << d["outcome"];
    if (d["result"]["outcome"] == "Warning") {
        msgs.push_back("Messsages:");
        json::iterator mIt;
        for (auto & v : d["result"]["messages"]) {
            msgs.push_back(v);
        }

        msgs.push_back("Values:");
        msgs.push_back(d["values"]);

        Alert * alert = new Alert(Alert::Diagnostics,
                                  Alert::Warning,
                                  Alert::Diagnostic,
                                  location,
                                  "",
                                  0);
        alert->setMessages(msgs);
        alert->setFile(fileName);
        issues.push_back(alert);
    }
}

// }
