#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================

project (fmk)

#-----------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (CURLINCDIR /usr/include)
set (CURLLIBDIR /usr/lib64)

set (PSQLDIR /usr/pgsql)
set (PSQLLIBDIR ${PSQLDIR}/lib)
set (PSQLINCDIR ${PSQLDIR}/include)
set (PSQLLIB pq)

set (PWD ${fmk_SOURCE_DIR})

set (JSON_ROOT_DIR    	${PWD}/../json)
set (HTTPSC_ROOT_DIR  	${PWD}/../httpsc)
set (INFIX_ROOT_DIR   	${PWD}/../infix)
set (TOOLS_ROOT_DIR   	${PWD}/../tools)
set (LOG_ROOT_DIR     	${PWD}/../log)
set (STR_ROOT_DIR     	${PWD}/../str)
set (Q_ROOT_DIR     	${PWD}/../q)
set (UUID_ROOT_DIR    	${PWD}/../uuid)
set (FILETOOLS_ROOT_DIR ${PWD}/../filetools)
set (FMK_ROOT_DIR       ${PWD}/../fmk)
set (VOS_ROOT_DIR       ${PWD}/../vos)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)

LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib
  ${CURLLIBDIR}
  ${PSQLLIBDIR})
#-----------------------------------------------------------------

set (libfmk_hdr
  datamng.h
  dckmng.h      
  cntrmng.h     
  srvmng.h
  cs.h
  dbhdl.h       
  dbhdlpostgre.h
  fifo.h
  fmt.h
  fnamespec.h
  fv.h
  master.h
  masterrequester.h
  masterserver.h
  procnet.h
  prodloc.h
  taskagent.h
  taskmng.h
  taskorc.h
  types.h
  wa.h
)

set (libfmk_src
  datamng.cpp
  dckmng.cpp    
  cntrmng.cpp   
  srvmng.cpp
  cs.cpp
  dbhdlpostgre.cpp
  fifo.cpp
  fnamespec.cpp
  fv.cpp
  master.cpp
  masterrequester.cpp
  masterserver.cpp
  procnet.cpp
  prodloc.cpp
  taskagent.cpp
  taskmng.cpp
  taskorc.cpp
  types.cpp
  wa.cpp
)

#===== Project sections/libraries =======

add_library(fmk SHARED ${libfmk_src})
target_include_directories (fmk PUBLIC . 
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${Q_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${HTOOLS_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${HTTPSC_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR}
  ${VOS_ROOT_DIR}
  ${PSQLINCDIR})

target_link_libraries (fmk
  httpsc
  filetools
  tools
  log
  q
  vos  
  ${PSQLLIB}
  log4cpp
  pthread
  curl)
set_target_properties (fmk PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmk
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)

