/******************************************************************************
 * File:    fnamespec.cpp
 *          This file is part of QPF
 *
 * Domain:  qpf.fmk.FileNameSpec
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
 *   Implement FileNameSpec class
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

#include "fnamespec.h"

#include "filetools.h"
#include "metadatareader.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
FileNameSpec::FileNameSpec() //: re(std::move(std::regex(BnameRe)))
{
}

//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
FileNameSpec::~FileNameSpec()
{
}

//----------------------------------------------------------------------
// Method: parse
//----------------------------------------------------------------------
bool FileNameSpec::parse(string & fullFileName, ProductMeta & meta)
{
    static const int
	Mission = 1,
	ProcFunc = 2,
	Instance = 3,
	Date = 4,
	Version = 5;
    
    // Get basic path, name, etc. info
    // For a file like /this/is/a/dir/myfile.1.0.0.fits
    // dname  => /this/is/a/dir
    // bname  => myfile.1.0.0.fits
    // name   => myfile
    // suffix => 1.0.0.fits
    // sname  => myfile.1.0.0
    // ext    => fits
    string dname, bname, name, suffix, sname, ext;
    std::tie(dname, bname, name,
	     suffix, sname, ext) = FileTools::fileinfo(fullFileName);

    meta.append("id", bname);
    dict fs;
    fs.append("full", fullFileName);
    fs.append("path", dname);
    fs.append("base", bname);
    fs.append("name", name);
    fs.append("sname", sname);
    fs.append("suffix", suffix);
    fs.append("ext", ext);
    meta.append("fileinfo", fs);
    meta.append("url", "file://" + fullFileName);
    meta.append("format", genProdFormat(ext));

    std::regex re(BnameRe);
    std::smatch matches;
    if (!std::regex_search(sname, matches, re)) { return false; }

    meta.append("mission",    matches[Mission].str());
    meta.append("proc_func",  matches[ProcFunc].str());
    meta.append("creator",    matches[ProcFunc].str());
    meta.append("instance",   matches[Instance].str());
    meta.append("start_time", matches[Date].str());
    meta.append("end_time",   matches[Date].str());
    meta.append("version",    matches[Version].str());

    parseInstance(matches[Instance].str(), meta);

    bool fileExists = FileTools::exists(fullFileName);
    meta.append("exists", fileExists ? "yes" : "no");
    meta.append("size", FileTools::fileSize(fullFileName));
    if (fileExists) {
	retrieveInternalMetadata(fullFileName, meta);
    }

    return true;
}

//----------------------------------------------------------------------
// Method: genProdFormat
//----------------------------------------------------------------------
std::string FileNameSpec::genProdFormat(string & ext)
{
    string format(ext);
    str::toUpper(format);
    return format;
}

//----------------------------------------------------------------------
// Method: parseInstance
//----------------------------------------------------------------------
void FileNameSpec::parseInstance(string inst, ProductMeta & meta)
{
    vector<string> additional;
    vector<string> insTokens;
    str::split(inst, '-', insTokens);
    string creator, expo, obsm, obsid;
    
    for (auto & token: insTokens) {
	if (token.length() == 1) {
	    if (SpectralBands.find(token) != string::npos) {
		meta.append("spectral_band", token);
	    } else if (str::isDigits(token)) {
		expo = token;
		meta.append("exposure", stoi(expo));
	    } else {
		obsm = token;
		meta.append("obs_mode", obsm);
	    }
	} else if (str::isDigits(token)) {
	    obsid = token;
	    meta.append("obs_id", obsid);
	} else if (Creators.find("-" + token + "-") != string::npos) {
	    creator = token;
	    meta.append("creator", creator);
	} else if (DataTypes.find("-" + token + "-") != string::npos) {
	    meta.append("data_type", token);
	} else {
	    additional.push_back(token);
	}
    }
    
    meta.append("additional", str::join(additional, "-"));

    string pf(meta["proc_func"].asString());
    string typ(pf + ((creator == pf) ? "" : ("_" + creator)));
    meta.append("type", typ);
    meta.append("instrument", typ.substr(typ.length() - 3));
    
    meta.append("signature", obsid + "-" + expo + "-" + obsm);
}

//----------------------------------------------------------------------
// Method: retrieveInternalMetadata
//----------------------------------------------------------------------
void FileNameSpec::retrieveInternalMetadata(string fileName, ProductMeta & meta)
{
    if (meta["format"].asString() == "FITS") {
	FitsMetadataReader fitsMD(fileName);
	string hdrMetaData;
	if (fitsMD.getMetadataInfoStr(hdrMetaData)) {
	    meta.append("meta", hdrMetaData);
	} else {
	    meta.append("meta", "<none>");
	}
    }	
}

const string FileNameSpec::BnameRe("([A-Z]{3,3})_"              // mission
				   "([A-Z0-9]{3,3})_"           // procfunc
				   "([^_]+)_"                   // instance
				   "(20[0-9]+T[\\.0-9]+Z)"      // date
				   "_*(([0-9]+\\.[0-9]+)*)");   // version
const string FileNameSpec::SpectralBands("UBVRIJHKLMNQGZY");
const string FileNameSpec::Creators("-NIR-SIR-VIS-");
const string FileNameSpec::DataTypes("-CAT-TRANS-STACK-MASK-MAP-"
				     "PSF-SPE1D-MAP2DCOR-TIPS-");

