#!/bin/bash

set -e

sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    catch2 \
    libboost-all-dev \
    libgmp-dev \
    libmpfr-dev \
    libmpfi-dev \
    flatbuffers-compiler \
    libflatbuffers-dev \
    cloc

echo "Setup finished"
