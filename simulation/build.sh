#!/bin/bash

project_path="$(pwd)"
export PYTHONPATH="${PYTHONPATH}:$project_path"

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
