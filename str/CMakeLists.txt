#======================================================================
# Str - Simple Strging library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

project (str)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (strLib_hdr
  str.h)

set (strLib_src
  str.cpp)

add_library (str SHARED ${strLib_src})
set_target_properties (str PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS str
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
