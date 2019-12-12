#======================================================================
# Str - Simple Queue class library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

project (q)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (qLib_hdr
  q.h)

set (qLib_src
  q.cpp)

add_library (q SHARED ${qLib_src})
set_target_properties (q PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS q
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
