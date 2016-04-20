#!/bin/bash

project_path=$(dirname $0)
project_path=$(readlink -e $project_path)
export PYTHONPATH="${PYTHONPATH}:$project_path"
echo $PYTHONPATH

echo "Building photoconversion..."
cd "$project_path/photoconversion"
mkdir -p "build" && cd "build"
cmake -DCMAKE_INSTALL_PREFIX=.. ..
make

echo "Building drift..."
cd "$project_path/drift"
make

echo "Building avalanche..."
cd "$project_path/avalanche"
make

echo "Done building..."
