#======================================================================
# Str - Simple interface to log4cpp library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

project (log)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (logLib_hdr
  log.h)

set (logLib_src
  log.cpp)

add_library (log SHARED ${logLib_src})
target_link_libraries (log log4cpp)
set_target_properties (log PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS log
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
