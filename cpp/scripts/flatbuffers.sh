#!/bin/bash

set -e

cd "$(dirname "${BASH_SOURCE[0]}")/../"

flatc --cpp -o src/flatbuffers/ src/flatbuffers.fbs

flatc --ts -o ../web/src/lib/flatbuffers/ src/flatbuffers.fbs