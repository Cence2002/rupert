#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ "$PWD" != "$SCRIPT_DIR" ]; then
  echo "ERROR: Script must be run from: $SCRIPT_DIR"
  echo "       Current directory:       $PWD"
  exit 1
fi

BUILD_DIR="build"
JOBS=4

rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ..
cmake --build . -- -j$JOBS
cd ..
