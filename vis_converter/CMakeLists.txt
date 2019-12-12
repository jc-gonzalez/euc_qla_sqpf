#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 3.7)


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

#===== COTS =====

if ("${COTSDIR}" STREQUAL "")

  set (NNMSG_ROOT_DIR /usr/local)
  set (NNMSGINCDIR ${NNMSG_ROOT_DIR}/include)
  set (NNMSGLIBDIR ${NNMSG_ROOT_DIR}/lib)

  set (UUIDINCDIR /usr/include)
  set (UUIDLIBDIR /usr/lib64)
  
  set (CURLINCDIR /usr/include)
  set (CURLLIBDIR /usr/lib64)
  
  set (PCRE2INCDIR /usr/include)
  set (PCRE2LIBDIR /usr/lib64)
  
  if (EXISTS /usr/pgsql)
    set (PSQLDIR /usr/pgsql)
    set (PSQLLIBDIR ${PSQLDIR}/lib)
    set (PSQLINCDIR ${PSQLDIR}/include)
  else()
    set (PSQLLIBDIR /usr/lib/x86_64-linux-gnu)
    set (PSQLINCDIR /usr/include/postgresql)
  endif()
  
else()

  message ("Using a custom COTSDIR = ${COTSDIR}")
  
  set (NNMSG_ROOT_DIR ${COTSDIR}/nanomsg)
  set (NNMSGINCDIR ${NNMSG_ROOT_DIR}/include)
  set (NNMSGLIBDIR ${NNMSG_ROOT_DIR}/lib64)

  set (UUID_ROOT_DIR ${COTSDIR}/uuid)
  set (UUIDINCDIR ${UUID_ROOT_DIR}/include)
  set (UUIDLIBDIR ${UUID_ROOT_DIR}/lib)
  
  set (CURL_ROOT_DIR ${COTSDIR}/curl)
  set (CURLINCDIR ${CURL_ROOT_DIR}/include)
  set (CURLLIBDIR ${CURL_ROOT_DIR}/lib)
  
  set (PCRE2_ROOT_DIR ${COTSDIR}/pcre2)
  set (PCRE2INCDIR ${PCRE2_ROOT_DIR}/include)
  set (PCRE2LIBDIR ${PCRE2_ROOT_DIR}/lib)
  
  set (PSQL_ROOT_DIR ${COTSDIR}/pgsql)
  set (PSQLINCDIR ${PSQL_ROOT_DIR}/include)
  set (PSQLLIBDIR ${PSQL_ROOT_DIR}/lib)

endif()

set (PCRE2LIB pcre2-posix)
set (UUIDLIB uuidxx)
set (PSQLLIB pq)

#===== Project libraries =====

#set (PWD /Users/jcgonzalez/ws/QPFnn)
#set (PWD ..)

set (PWD ${sQPF_SOURCE_DIR})

set (JSON_ROOT_DIR    ${PWD}/json)
set (NNCOMM_ROOT_DIR  ${PWD}/nncomm)
set (INFIX_ROOT_DIR   ${PWD}/infix)
set (TOOLS_ROOT_DIR   ${PWD}/tools)
set (LOG_ROOT_DIR     ${PWD}/log)
set (STR_ROOT_DIR     ${PWD}/str)
set (UUID_ROOT_DIR    ${PWD}/uuid)
set (FILETOOLS_ROOT_DIR ${PWD}/filetools)
set (FMK_ROOT_DIR     ${PWD}/fmk)
set (VOS_ROOT_DIR     ${PWD}/vos)

set (MONGOOSEDIR      ${PWD}/mongoose)

#==== Common directives


INCLUDE_DIRECTORIES(
  .
  /usr/include
  $ENV{HOME}/opt/include
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  )

LINK_DIRECTORIES (
  /usr/lib64
  /usr/lib
  $ENV{HOME}/opt/lib
  ${CURLLIBDIR}
  ${FILETOOLS_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${PSQLLIBDIR}
#  ${NNMSGLIBDIR}
#  ${UUIDLIBDIR}
#  ${PCRE2LIBDIR}
#  ${NNCOMM_ROOT_DIR}
#  ${LOG_ROOT_DIR}
#  ${UUID_ROOT_DIR}
)
#-----------------------------------------------------------------

project (vis_converter)

set (vis_converter_hdr
ccd_id.h
compr_info.h
config_tables.h
except.h
expo_duration.h
le1_vis_ccd.h
#log.h
q.h
raw_vis_hdr.h
raw_vis_scipckt.h
tcparams.h
types.h
ver_seqconf.h
vis_base.h
vis_converter.h
)

set (vis_converter_src
main.cpp
ccd_id.cpp
compr_info.cpp
config_tables.cpp
expo_duration.cpp
le1_vis_ccd.cpp
#log.cpp
q.cpp
raw_vis_hdr.cpp
raw_vis_scipckt.cpp
tcparams.cpp
types.cpp
ver_seqconf.cpp
vis_base.cpp
vis_converter.cpp
)

set (SZIP_DIR /usr/local/Cellar/szip/2.1.1_1)
set (SZIP_INC_DIR ${SZIP_DIR}/include)
set (SZIP_LIB_DIR ${SZIP_DIR}/lib)

set (CFITSIO_DIR /usr/local/Cellar/cfitsio/3.450)
set (CFITSIO_INC_DIR ${CFITSIO_DIR}/include)
set (CFITSIO_LIB_DIR ${CFITSIO_DIR}/lib)

include_directories(${SZIP_INC_DIR} ${CFITSIO_INC_DIR})
link_directories(${SZIP_LIB_DIR} ${CFITSIO_LIB_DIR})

add_executable(vis_converter ${vis_converter_src})
target_include_directories (vis_converter PUBLIC . ..
  ${SZIP_INC_DIR}
  ${CFITSIO_INC_DIR}
)
target_link_libraries (vis_converter sz curl cfitsio m)

set_target_properties (vis_converter PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS vis_converter
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
