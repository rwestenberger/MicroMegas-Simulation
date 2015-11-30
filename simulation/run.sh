#!/bin/bash

project_path="$(pwd)"

echo "Running photoconversion..."
"$project_path/photoconversion/build/photoconversion"

echo "Running drift..."
"$project_path/drift/drift"

echo "Running avalanche..."
"$project_path/avalanche/avalanche"