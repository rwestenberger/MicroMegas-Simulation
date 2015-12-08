#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage: $(basename $0) DIRECTORY"
	exit 1
fi

WD="$1"

INPUT_FILE="photoconversion.root"
AVAL_EXEC="/home/rwestenb/simulation/MicroMegas-Simulation/simulation/avalanche/avalanche"

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

# GNU Parallel
module load software/gnu_parallel

# run parallel on all split files
# needs about ?M RAM per job
find ${WD} -regextype posix-egrep -regex '^.*photoconversion_[0-9]+_out\.root$' | parallel -j 64 --delay 1 --progress --no-notice "$AVAL_EXEC {} {.}_aval.root > {.}_aval.txt"

STATUS=$?
exit $STATUS

