#!/bin/bash

project_path="$(pwd)"

echo "Running photoconversion..."
cd "$project_path/photoconversion/build/"
./photoconversion

echo "Running drift..."
cd "$project_path/drift/"
./drift

echo "Running avalanche..."
cd "$project_path/avalanche/"
./avalanche
