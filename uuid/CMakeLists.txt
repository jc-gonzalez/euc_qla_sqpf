#======================================================================
# Uuid - Simple Uuid wrapper library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

project (uuidxx)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (PWD ${uuidx_SOURCE_DIR})

set (HTOOLS_ROOT_DIR    ${PWD}/../htools)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include
  ${HTOOLS_ROOT_DIR})
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (uuidxxLib_hdr
  uuidxx.h)

set (uuidxxLib_src
  uuidxx.cpp)

link_directories (${UUIDLIBDIR})
include_directories (${UUIDINCDIR})

add_library (uuidxx SHARED ${uuidxxLib_src})
target_include_directories (uuidxx PUBLIC .)
target_link_libraries (uuidxx ${UUIDLIB})
set_target_properties (uuidxx PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS uuidxx
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
