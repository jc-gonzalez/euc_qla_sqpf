/******************************************************************************
 * File:    filetools.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.FileTools
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
 *   Provides object implementation for FileTools namespace functions
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

#include "filetools.h"

#include "str.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dirent.h>

#if defined(__APPLE__) && defined(__MACH__)
#   warning "Compiling for Mac OS X . . ."
#   include <copyfile.h>
#else
#   include <sys/sendfile.h>
#endif

#include <iostream>
#include <fstream>

//#include "dbg.h"

//======================================================================
// Namespace: FileTools
// File releated utility functions
//======================================================================
namespace FileTools {

//------------------------------------------------------------
// Method: generateTmpFile
// Generate a temporary file with unique file name in /tmp
//------------------------------------------------------------
std::string generateTmpFile()
{
    // Prepare temporary file
    char buff[64];
    strcpy(buff, "/tmp/tmpXXXXXX");
    int fileDes = mkstemp(buff);
    if (fileDes > 0) { close(fileDes); }
    return std::string(buff);
}

//------------------------------------------------------------
// Method: storeFileIntoString
// Handy short function to store the entire content of a file
// into a string
//------------------------------------------------------------
void storeFileIntoString(std::string & iFile, std::string & s)
{
    FILE* f = fopen(iFile.c_str(), "rb");

    // Determine file size
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    char * where = new char[size];
    fread(where, sizeof(char), size, f);

    s.assign((const char*)(where), size);

    delete[] where;
}

//----------------------------------------------------------------------
// Method: existsDir
// Removes old log and msg files
//----------------------------------------------------------------------
bool exists(std::string pathName)
{
    struct stat sb;
    return (stat(pathName.c_str(), &sb) == 0);
}

//----------------------------------------------------------------------
// Method: existsDir
// Removes old log and msg files
//----------------------------------------------------------------------
bool existsDir(std::string pathName)
{
    struct stat sb;
    return (stat(pathName.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

//----------------------------------------------------------------------
// Method: fileSize
// Returns the size of the file
//----------------------------------------------------------------------
int fileSize(std::string pathName)
{
    struct stat sb;
    return (stat(pathName.c_str(), &sb) == 0) ? sb.st_size : -1;
}

//----------------------------------------------------------------------
// Method: filesInFolder
//----------------------------------------------------------------------
std::vector<std::string> filesInFolder(std::string folder, std::string ext)
{
    std::vector<std::string> v;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder.c_str())) != NULL) {
	/* print all the files and directories within directory */
	while ((ent = readdir(dir)) != NULL) {
	    std::string filename(ent->d_name);
	    if (!ext.empty()) {
		std::string entext(str::getExtension(filename));
		if (entext != ext) continue;		    
	    }
	    v.push_back(folder + "/" + filename);
	}
	closedir(dir);
    } else {
	/* could not open directory */
	perror("filesInFolder");
    }
    return v;
}
    
//----------------------------------------------------------------------
// Method: copyfile
//----------------------------------------------------------------------
int copyfile(std::string & sFrom, std::string & sTo)
{
#if defined(__APPLE__) && defined(__MACH__)
    copyfile_flags_t flags = COPYFILE_DATA;
    return ::copyfile(sFrom.c_str(), sTo.c_str(), NULL, flags);
#else
    int source = open(sFrom.c_str(), O_RDONLY, 0);
    int dest = open(sTo.c_str(), O_WRONLY | O_CREAT, 0644);

    // struct required, rationale: function stat() exists also
    struct stat stat_source;
    fstat(source, &stat_source);

    //TRC("Local copying: " + sFrom + " => " + sTo);
    sendfile(dest, source, 0, stat_source.st_size);

    close(source);
    close(dest);
#endif
    return 0;
}

//----------------------------------------------------------------------
// Method: rcopyfile
//----------------------------------------------------------------------
int rcopyfile(std::string & sFrom, std::string & sTo,
              std::string & remoteHost, bool fromRemote)
{
    static std::string scp("/usr/bin/scp");
    std::string cmd;
    if (fromRemote) {
        cmd = scp + " " + remoteHost + ":" + sFrom + " " + sTo;
    } else {
        cmd = scp + " " + sFrom + " " + remoteHost + ":" + sTo;
    }
    int res = system(cmd.c_str());
    //TRC("Remote copying: " + cmd);
    (void)(res);

    return 0;
}

//----------------------------------------------------------------------
// Method: runlink
//----------------------------------------------------------------------
int runlink(std::string & f, std::string & remoteHost)
{
    std::string cmd;
    cmd = "ssh " + remoteHost + " rm " + f;
    int res = system(cmd.c_str());
    //TRC("Remote unlinking: " + cmd);
    (void)(res);

    return 0;
}

//----------------------------------------------------------------------
// Method: fileinfo
//----------------------------------------------------------------------
std::tuple<std::string, std::string,
	   std::string, std::string,
	   std::string, std::string> fileinfo(std::string fname)
{
    size_t ibn = fname.find_last_of("/\\");
    std::string bname = fname.substr(ibn + 1);
    std::string dname = fname.substr(0, ibn);
    size_t idtf = bname.find_first_of('.');
    size_t idtl = bname.find_last_of('.');
    std::string name = bname.substr(0, idtf);
    std::string sname = bname.substr(0, idtl);
    std::string suffix = bname.substr(idtf + 1);
    std::string ext = bname.substr(idtl + 1);

    return make_tuple(dname, bname, name, suffix, sname, ext);
}

}
