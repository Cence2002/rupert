#!/bin/bash

cd "$(dirname "${BASH_SOURCE[0]}")/../"

SOURCE_DIR="src"
CPP_FILE="temp.cpp"

for HEADER_PATH in $(find $SOURCE_DIR -name '*.hpp'); do
  echo "#include \"$HEADER_PATH\"" > $CPP_FILE
  g++ -std=c++20 -fsyntax-only -w -I $SOURCE_DIR -c $CPP_FILE
  rm $CPP_FILE
  echo "Compiled: $HEADER_PATH"
done
