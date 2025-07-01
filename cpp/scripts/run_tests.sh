#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

BUILD_DIR=".build_tests"
EXECUTABLE="rupert_tests"

cd $BUILD_DIR
./$EXECUTABLE "$@"