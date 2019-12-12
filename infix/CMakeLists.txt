#======================================================================
# InFix - Simple Infix Expressions Evaluation Library
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

project (infix)

set (infixLib_hdr
  infixeval.h)

add_library (infix SHARED ${infixLib_hdr})
set_target_properties (infix PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS infix
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
