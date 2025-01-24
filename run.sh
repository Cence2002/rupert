#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ "$PWD" != "$SCRIPT_DIR" ]; then
  echo "ERROR: Script must be run from: $SCRIPT_DIR"
  echo "       Current directory:       $PWD"
  exit 1
fi

BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
  echo "Error: Build directory not found '$BUILD_DIR'"
  exit 1
fi

EXECUTABLE="rupert"
if [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
  echo "Error: Executable not found: '$BUILD_DIR/$EXECUTABLE'"
  exit 1
fi

./$BUILD_DIR/$EXECUTABLE
