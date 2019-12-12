#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
cmake_minimum_required(VERSION 3.7)

include (../common.cmake)

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
