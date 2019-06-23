#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

project (filetools)

set (libfiletools_hdr
  filetools.h
  genfhdl.h
  jsonfhdl.h
  qdtrephdl.h
)

set (libfiletools_src
  filetools.cpp
  jsonfhdl.cpp
  qdtrephdl.cpp
)

#===== Project sections/libraries =======

add_library(filetools SHARED ${libfiletools_src})
target_include_directories (filetools PUBLIC .
#  ${PSQLINCDIR}
#  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
#  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR}
#  ${PCRE2LIBDIR}
#  ${PSQLLIBDIR}
#  ${MONGOOSEDIR}
  )
#target_link_libraries (filetools
  #json infix str log tools
  #nncomm nanomsg mongoose
  # ${PCRE2LIB} ${PSQLLIB}
  #uuid pthread
#  )
set_target_properties (filetools PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS filetools
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
