#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================

project (qpf)

#-----------------------------------------------------------------
#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (NNMSG_ROOT_DIR /usr/local)
set (NNMSGINCDIR ${NNMSG_ROOT_DIR}/include)
set (NNMSGLIBDIR ${NNMSG_ROOT_DIR}/lib)

set (UUIDINCDIR /usr/include)
set (UUIDLIBDIR /usr/lib64)
set (UUIDLIB uuidxx)

set (CURLINCDIR /usr/include)
set (CURLLIBDIR /usr/lib64)

set (PCRE2INCDIR /usr/include)
set (PCRE2LIBDIR /usr/lib64)
  
set (PSQLDIR /usr/pgsql)
set (PSQLLIBDIR ${PSQLDIR}/lib)
set (PSQLINCDIR ${PSQLDIR}/include)
set (PSQLLIB pq)

set (PCRE2LIB pcre2-posix)

set (PWD ${qpf_SOURCE_DIR})

set (JSON_ROOT_DIR    	${PWD}/../json)
set (NNCOMM_ROOT_DIR  	${PWD}/../nncomm)
set (INFIX_ROOT_DIR   	${PWD}/../infix)
set (TOOLS_ROOT_DIR   	${PWD}/../tools)
set (LOG_ROOT_DIR     	${PWD}/../log)
set (Q_ROOT_DIR       	${PWD}/../q)
set (STR_ROOT_DIR     	${PWD}/../str)
set (UUID_ROOT_DIR    	${PWD}/../uuid)
set (FILETOOLS_ROOT_DIR ${PWD}/../filetools)
set (FMK_ROOT_DIR     	${PWD}/../fmk)
set (VOS_ROOT_DIR     	${PWD}/../vos)

INCLUDE_DIRECTORIES (. /usr/include $ENV{HOME}/opt/include)
LINK_DIRECTORIES (/usr/lib64 /usr/lib $ENV{HOME}/opt/lib
  ${CURLLIBDIR}
  ${PSQLLIBDIR})

#-----------------------------------------------------------------

set (qpf_hdr
  deployer.h
)

set (qpf_src
  main.cpp
  deployer.cpp
)

add_executable(qpf ${qpf_src})
target_include_directories (qpf PUBLIC . ..
  ${Q_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${PSQLINCDIR}
  )
target_link_libraries (qpf
  q fmk infix str filetools tools httpsc
  httpserver ${PSQLLIB} log4cpp pthread curl)
set_target_properties (qpf PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS qpf
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
