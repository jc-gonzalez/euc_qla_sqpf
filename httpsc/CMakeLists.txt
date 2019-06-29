#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

project (httpsc)

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
  ${CURLINCDIR}
  ${STR_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
#  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR})
target_link_libraries (httpsc
  tools 
  str 
#  json 
  filetools
  httpserver
  pthread curl)
set_target_properties (httpsc PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS httpsc
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
