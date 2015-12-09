#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo "Usage: $(basename $0) DEST_FILE SOURCE_FILE [SOURCE_FILE]"
	exit 1
fi

# ROOT setup
source /cluster/cern/root_v5.34.03_x86_64/bin/thisroot.sh

$ROOTSYS/bin/hadd -f $@

STATUS=$?
exit $STATUS

