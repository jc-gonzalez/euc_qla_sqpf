#!/bin/bash

APP=Tools
DOMAIN=tools
COPY="2015-2020 Euclid SOC Team / J C Gonzalez"

cls="VIS_Base"
mod="vis_base"
mths=""
~/bin/cpp/createC++Module.sh "$mod" "$cls" "$APP" "$DOMAIN" "$COPY" "$mths"

cls="RAW_VIS_Header"
mod="raw_vis_hdr"
mths="%void#read()"
mths="$mths|string#info()"
mths="$mths|bool#checkCRC()"
~/bin/cpp/createC++Module.sh "$mod" "$cls" "$APP" "$DOMAIN" "$COPY" "$mths"

cls="RAW_VIS_SciPacket"
mod="raw_vis_scipckt"
mths="%void#read(o)"
mths="$mths|string#info()"
mths="$mths|bool#readHeader()"
mths="$mths|bool#readData()"
mths="$mths|bool#readFooter()"
mths="$mths|bool#checkCRC()"
mths="$mths|bool#uncompressData()"
mths="$mths|bool#reorderData()"
~/bin/cpp/createC++Module.sh "$mod" "$cls" "$APP" "$DOMAIN" "$COPY" "$mths"

cls="VIS_Converter"
mod="vis_cnv"
mths="$mths|void#readInput()"
mths="$mths|void#writeOutput()"
~/bin/cpp/createC++Module.sh "$mod" "$cls" "$APP" "$DOMAIN" "$COPY" "$mths"


~/bin/cpp/createC++Module.sh "compr_mode" "ComprMode" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "compr_type" "ComprType" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "compr_info" "CompressionInfo" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "expo_duration" "ExposureDuration" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "ver_seqconf" "VersionSeqConf" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "config_tables" "ConfigTables" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "tcparams" "TCParameters" "$APP" "$DOMAIN" "$COPY" "%"
~/bin/cpp/createC++Module.sh "ccd_id" "CcdId" "$APP" "$DOMAIN" "$COPY" "%"
