#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage: $(basename $0) DIRECTORY"
	exit 1
fi

WD="$1"

INPUT_FILE="photoconversion.root"
SPLIT_SCRIPT="/home/rwestenb/simulation/MicroMegas-Simulation/outfiles/scripts/splitFile.py"

echo "Using simulation directory: $WD"

if [ ! -e "$WD/$INPUT_FILE" ];
then
	echo "Input file: $INPUT_FILE does not exist!"
	exit 1
fi

# ROOT and Garfield++ setup
source /cluster/cern/root_v5.34.03_x86_64/bin/thisroot.sh
export GARFIELD_HOME=/home/rwestenb/simulation/software/Garfield
export HEED_DATABASE=$GARFIELD_HOME/Heed/heed++/database

$SPLIT_SCRIPT -j 64 -t coatingTree $WD/$INPUT_FILE > $WD/split.log

STATUS=$?
exit $STATUS

