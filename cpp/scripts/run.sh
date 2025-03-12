#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

BUILD_DIR="build"
EXECUTABLE="rupert"

cd $BUILD_DIR
./$EXECUTABLE "$@"