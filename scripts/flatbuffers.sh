#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

flatc --cpp -o cpp/src/flatbuffers/ cpp/src/flatbuffers.fbs

flatc --ts -o web/src/lib/flatbuffers/ cpp/src/flatbuffers.fbs
