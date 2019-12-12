#======================================================================
# VOSHdl - VOSpace Handlers
#======================================================================
# Library project file
# Author: J C Gonzalez - 2015-2018
#======================================================================

project (vos)

#-----------------------------------------------------------------
cmake_minimum_required (VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (PWD ${vos_SOURCE_DIR})

set (FILETOOLS_ROOT_DIR ${PWD}/../filetools)
set (STR_ROOT_DIR       ${PWD}/../str)
set (HTOOLS_ROOT_DIR    ${PWD}/../htools)
set (HTTPSC_ROOT_DIR    ${PWD}/../httpsc)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib)
#-----------------------------------------------------------------

set (vosLib_hdr
  voshdl.h)

set (vosLib_src
  voshdl.cpp)

#find_package(CURL REQUIRED)

add_library (vos SHARED ${vosLib_src})
set_target_properties (vos PROPERTIES LINKER_LANGUAGE CXX)
include_directories(. 
  ${FILETOOLS_ROOT_DIR} ${STR_ROOT_DIR} 
  ${HTOOLS_ROOT_DIR} ${HTTPSC_ROOT_DIR} 
  ${CURLINCDIR})
link_directories (${CURLLIBDIR})
target_include_directories (vos PUBLIC .)
target_link_libraries(vos curl)
install (TARGETS vos
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)

