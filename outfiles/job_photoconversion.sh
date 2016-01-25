#!/bin/bash

if [ "$#" -ne 1 ]; then
        echo "Usage: $(basename $0) DIRECTORY"
        exit 1
fi

if [ ! -e "$1" ]; then
	echo "Directory $1 does not exist, creating..."
	mkdir "$1"
fi

WD="$1"

PHOTOCONVERSION_EXEC="/home/rwestenb/simulation/MicroMegas-Simulation/simulation/photoconversion/build/photoconversion"

echo "Using simulation directory: $WD"

# ROOT and Garfield++ setup
source /cluster/cern/root_v5.34.03_x86_64/bin/thisroot.sh

# needs about 300M ram for 100k photons
$PHOTOCONVERSION_EXEC > $WD/photoconversion.log

STATUS=$?
exit $STATUS

