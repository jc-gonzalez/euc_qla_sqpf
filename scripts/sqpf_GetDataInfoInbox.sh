#!/bin/bash
######################################################################################
# QPF_GetDataIntoInbox.sh
# Store data from folders specified at command line into the inbox, at an specified
# rate
######################################################################################

#-- Script variables

#- This script path and name
SCRIPT_PATH="${BASH_SOURCE[0]}";
SCRIPT_NAME=$(basename "${SCRIPT_PATH}")
if [ -h "${SCRIPT_PATH}" ]; then
    while [ -h "${SCRIPT_PATH}" ]; do
        SCRIPT_PATH=$(readlink "${SCRIPT_PATH}")
    done
fi
pushd . > /dev/null
cd $(dirname ${SCRIPT_PATH}) > /dev/null
SCRIPT_PATH=$(pwd)
popd  > /dev/null

FOLDERS=""
TIMEINTERVAL=30
INTERACTIVE="no"
INBOX="."
ARCHIVE="../archive/"

#-- Other
DATE=$(date +"%Y%m%d%H%M%S")
LOG_FILE=./${DATE}.log
DATA_LIST=./${DATE}.lst
ARCH_LIST=./${DATE}.arc
MAX_FILES_IN_ARCHIVE=99

#-- Define functions
usage () {
    echo "Usage:"
    echo "        ${SCRIPT_NAME} -i <inbox>  -a <archive>  -d <folder> [ -d <folder> [..]] [ -t <secs> ] [ -I ]"
    echo ""
}

#-- Get options from command line
while getopts :hd:t:Ii: OPT; do
    case $OPT in
        h|+h) usage ;;
        d|+d) FOLDERS="$FOLDERS $OPTARG" ;;
        t|+t) TIMEINTERVAL=$OPTARG ;;
        a|+a) ARCHIVE=$OPTARG ;;
        i|+i) INBOX=$OPTARG ;;
        I|+I) INTERACTIVE="yes" ;;
        *)    usage ; exit 2
    esac
done
shift `expr $OPTIND - 1`
OPTIND=1

if [ -z "$FOLDERS" ]; then
    usage
    echo "Please, specify at least one folder to get EUC....fits files"
    exit 3
fi

#-- Get files
cat /dev/null > ${DATA_LIST}.orig
for d in $FOLDERS; do
    ls -1 ${d}/EUC_*.fits >> ${DATA_LIST}.orig
done

#-- Shuffle the list
while read line; do echo $RANDOM $line; done < ${DATA_LIST}.orig | \
    sort -n | cut -f2- -d' ' > ${DATA_LIST}

numFiles=$(wc -l ${DATA_LIST}|awk '{print $1;}')

#-- Show header
DATE=$(date +"%Y-%m-%d %H:%M:%S")

echo "==============================================================="
echo "Automatic insertion of data files into sQPF inbox"
echo "==============================================================="

echo "Starting at $DATE"
echo "List of Folders:"
for d in ${FOLDERS}; do
    echo " - $d"
done
echo "Interactive mode: $INTERACTIVE"
echo "Time interval:    $TIMEINTERVAL"

qpfpid=$(pidof qpf)

#======== MAIN LOOP

numFile=0

if [ -f "seq.dat" ]; then
    seqNum=$(cat seq.dat)
else
    seqNum=0
fi

mkdir ${ARCHIVE}.old

while [ -d . ]; do

    # Increase index of file to take from list
    numFile=$((numFile + 1))
    if [ $numFile -gt $numFiles ]; then
        numFile=0
        continue
    fi

    # Get file from list and split in fields
    dataFile=$(awk -v line=$numFile '(NR==line){print;next;}' ${DATA_LIST})
    bFile=$(basename $dataFile)
    dFile=$(dirname $dataFile)
    pre=$(echo $bFile | cut -c 1-12)
    obsid=$(echo $bFile | cut -c 13-17)
    post=$(echo $bFile | cut -c 18-)

    # Increase seqNum
    seqNum=$((seqNum + 1))
    echo $seqNum > "seq.dat"
    seqNumField=$(printf "%05d" $seqNum)

    # Create file name for inbox
    inboxFile=${INBOX}/${pre}${seqNumField}${post}

    # Create link
    ln ${dataFile} ${inboxFile}
    DATE=$(date +"%Y-%m-%d %H:%M:%S")
    echo "[${DATE}] $dataFile  ==>  ${inboxFile}"

#    # Rotate logs
#    k=1
#    while [ $k -lt 21 ]; do 
#        kk=$(( k + 1 ))
#        if [ -f $kk.log ]; then 
#            mv $kk.log $k.log
#        fi 
#    k=$kk 
#    done
#    thelog=$k.log

    # Check Local Archive size
    ls -1tr ${ARCHIVE}/ > $ARCH_LIST
    wc -l ${ARCH_LIST} > ${ARCH_LIST}.count
    read lines file < ${ARCH_LIST}.count
    if [ "$lines" -gt ${MAX_FILES_IN_ARCHIVE} ]; then
        excess=$(( lines - MAX_FILES_IN_ARCHIVE ))
        for f in $(head -${excess} $ARCH_LIST) ; do
            ls -l $f > ${ARCHIVE}.old/$(basename $f).ls
            rm $f
        done
    fi
        
    # Save log info
    ( ls -l /proc/$qpfpid/fd; \
      cat /proc/$qpfpid/{stat,statm,maps,smaps,io,sched}; \
    ) > $LOG_FILE

    # Wait until next
    if [ "$INTERACTIVE" == "no" ]; then
        sleep ${TIMEINTERVAL}
    else
        read ans
    fi
    
    # Check that QPF is still running
    qpfname=$(ps -q $qpfpid -o comm=)
    if [ -z "$qpfname" ]; then
        DATE=$(date +"%Y-%m-%d %H:%M:%S")
        echo "[{$DATE}] QPF not found running. Exiting."
        exit 1
    fi

done
