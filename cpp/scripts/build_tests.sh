#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

BUILD_DIR=".build_tests"

rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cmake -B $BUILD_DIR -S .
cmake --build $BUILD_DIR --target rupert_tests