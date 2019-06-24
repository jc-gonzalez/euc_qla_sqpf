#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# General Project File
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018 Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

project (fmk)

set (libfmk_hdr
  datamng.h
  dckmng.h      
  cntrmng.h     
  srvmng.h      
  dbhdl.h       
  dbhdlpostgre.h
  fifo.h
  fmt.h
  fnamespec.h
  log.h
  master.h
  masterrequester.h
  masterserver.h
  procnet.h
  prodloc.h
  q.h
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
  dbhdlpostgre.cpp
  fifo.cpp
  fnamespec.cpp
  log.cpp
  master.cpp
  masterrequester.cpp
  masterserver.cpp
  procnet.cpp
  prodloc.cpp
  q.cpp
  taskagent.cpp
  taskmng.cpp
  taskorc.cpp
  types.cpp
  wa.cpp
)

#===== Project sections/libraries =======

add_library(fmk SHARED ${libfmk_src})
target_include_directories (fmk PUBLIC . ../httpsc
  ${FMK_ROOT_DIR}
  ${JSON_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${FILETOOLS_ROOT_DIR}
  ${VOS_ROOT_DIR}
  ${PSQLINCDIR}
#  ${LOG_ROOT_DIR}
#  ${INFIX_ROOT_DIR}
#  ${NNCOMM_ROOT_DIR}
#  ${STR_ROOT_DIR}
#  ${UUID_ROOT_DIR}
#  ${NNMSGINCDIR}
#  ${PCRE2INCDIR}
#  ${UUIDINCDIR}
#  ${MONGOOSEDIR}
)

target_link_libraries (fmk
  json
  httpsc
  filetools
  tools
  vos
#  log
#  filehdl infix str log tools
#  uuidxx nncomm uuid vos
#  nanomsg mongoose
#  ${PCRE2LIB} pcre2-8
  ${PSQLLIB}
  log4cpp
  pthread)
set_target_properties (fmk PROPERTIES LINKER_LANGUAGE CXX)
install (TARGETS fmk
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)

