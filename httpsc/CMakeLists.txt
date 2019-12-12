#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================

project (httpsc)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (CURLINCDIR /usr/include)
set (CURLLIBDIR /usr/lib64)
  
set (TOOLS_ROOT_DIR      ../tools)
set (STR_ROOT_DIR        ../str)
set (FILETOOLS_ROOT_DIR  ../filetools)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (libhttpsc_hdr
  httpcommrqst.h
  httpcommsrv.h
  rwc.h
)

set (libhttpsc_src
  httpcommrqst.cpp
  httpcommsrv.cpp
  rwc.cpp
)

find_package(CURL REQUIRED)

add_library(httpsc SHARED ${libhttpsc_src})
target_include_directories (httpsc PUBLIC .
  ${STR_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR}
  ${CURLINCDIR})
target_link_libraries (httpsc str filetools 
  httpserver curl pthread)
set_target_properties (httpsc PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS httpsc
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
