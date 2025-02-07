#!/bin/bash

flatc --cpp -o src/flatbuffers/ src/flatbuffers.fbs

flatc --ts -o ../web/src/lib/flatbuffers/ src/flatbuffers.fbs