#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

BUILD_DIR="build"

rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cmake -B "$BUILD_DIR" -S .
cmake --build "$BUILD_DIR"

#ctest --test-dir "$BUILD_DIR" --output-on-failure

cd "$BUILD_DIR"
./tests
./rupert "$@"