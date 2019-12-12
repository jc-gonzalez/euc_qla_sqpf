/******************************************************************************
 * File:    main.cpp
 *          This file is part of Tools
 *
 * Domain:  tools.vis_converter
 *
 * Version:  2.0
 *
 * Date:    2019/11/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2020 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Parse an input VIS RAW file
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

//======================================================================
// Group: External Dependencies
//======================================================================

//----------------------------------------------------------------------
// Topic: System headers
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Topic: External packages
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Topic: Project headers
//----------------------------------------------------------------------
#include "vis_converter.h"

//----------------------------------------------------------------------
// Define options for arguments parser
//
// The fields are:
//  optletter, optname, type, helpstr, [NO_ARG|REQ_ARG|OPT_ARG]
//
// Each option will have the corresponding variable in the
// structure Args, of the type specified.  The argument, if
// present/required, is set ot the type of the variable. In case
// of bool, several options are checked. See the code.
//----------------------------------------------------------------------
#define ARGS_CMDNAME "vis_converter"
#define ARGS_DESC "Program to convert between types of input VIS data files"
#define ARGS_OPTIONS_LIST                                         \
    T('i', input_file, string, "Input file name", REQ_ARG)        \
    T('o', output_file, string, "Output file name", REQ_ARG)      \
    T('I', input_type, string, "Input file type", REQ_ARG)        \
    T('O', output_type, string, "Output file type", REQ_ARG)
//----------------------------------------------------------------------
#include "parse_args.h"

//----------------------------------------------------------------------
// Method: processCmdLineOpts
// Processes command line options to configure execution
//----------------------------------------------------------------------
bool processArgs(int argc, char * argv[], VisArgs & args)
{
    Args a;

    if (! parseArgs(argc, argv, a)) {
        std::cout << "\t\tValid input file types:\n";
        for (auto &kv : InputTypeDesc) {
            std::cout << "\t\t\t" << kv.first << " - " << kv.second << '\n';
        }
        std::cout << "\t\tValid output file types:\n";
        for (auto & kv: OutputTypeDesc) {
            std::cout << "\t\t\t" << kv.first << " - " << kv.second << '\n';
        }
        return false;
    }

    args.iFile = a.input_file,  args.iType = a.input_type;
    args.oFile = a.output_file, args.oType = a.output_type;

    return true;
}

//----------------------------------------------------------------------
// Function: main
// Creates the application main class, and invokes its run method
//----------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int exitCode = EXIT_SUCCESS;

    VisArgs args;
    if (! processArgs(argc, argv, args)) { return EXIT_FAILURE; }

    VIS_Converter vis_cnv;
    if (! vis_cnv.run(args)) {
        std::cerr << "ERROR: Conversion process failed!\n";
        exitCode = EXIT_FAILURE;
    };

    return exitCode;
}
