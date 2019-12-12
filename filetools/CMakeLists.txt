#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================

project (filetools)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (PWD ${filetools_SOURCE_DIR})

set (HTOOLS_ROOT_DIR ${PWD}/../htools)
set (TOOLS_ROOT_DIR  ${PWD}/../tools)
set (STR_ROOT_DIR    ${PWD}/../str)
set (JSON_ROOT_DIR   ${PWD}/../json)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (libfiletools_hdr
  filetools.h
  genfhdl.h
  jsonfhdl.h
  qdtrephdl.h
)

set (libfiletools_src
  filetools.cpp
  jsonfhdl.cpp
  qdtrephdl.cpp
)

#===== Project sections/libraries =======

add_library(filetools SHARED ${libfiletools_src})
target_include_directories (filetools PUBLIC .
  ${HTOOLS_ROOT_DIR} ${TOOLS_ROOT_DIR} ${STR_ROOT_DIR} ${JSON_ROOT_DIR})
set_target_properties (filetools PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS filetools
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
