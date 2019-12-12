#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================

project (tools)

#-----------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (UUIDINCDIR /usr/include)
set (UUIDLIBDIR /usr/lib64)
set (UUIDLIB uuidxx)
  
set (CURLINCDIR /usr/include)
set (CURLLIBDIR /usr/lib64)

set (PSQLDIR /usr/pgsql)
set (PSQLLIBDIR ${PSQLDIR}/lib)
set (PSQLINCDIR ${PSQLDIR}/include)
set (PSQLLIB pq)

set (PWD ${tools_SOURCE_DIR})

set (HTOOLS_ROOT_DIR    ${PWD}/../htools)
set (TOOLS_ROOT_DIR     ${PWD}/../tools)
set (JSON_ROOT_DIR      ${PWD}/../json)
set (STR_ROOT_DIR       ${PWD}/../str)
set (UUID_ROOT_DIR      ${PWD}/../uuid)
set (FILETOOLS_ROOT_DIR ${PWD}/../filetools)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

project (tools)

set (libtools_hdr
  alert.h
  bt.h
  dwatcher.h
  launcher.h
  metadatareader.h
  sm.h
  timer.h
  tools.h
)

set (libtools_src
  alert.cpp
  bt.cpp
  dwatcher.cpp
  launcher.cpp
  metadatareader.cpp
  sm.cpp
  timer.cpp
  tools.cpp
)

#find_package(CURL REQUIRED)

add_library(tools SHARED ${libtools_src})
target_include_directories (tools PUBLIC .
  ${CURLINCDIR}
  ${STR_ROOT_DIR}
  ${HTOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR})
target_link_libraries (tools
  pthread curl)
set_target_properties (tools PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS tools
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
