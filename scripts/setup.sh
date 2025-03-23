#!/bin/bash

set -e

sudo apt update
sudo apt install libboost-all-dev -y
sudo apt install libgmp-dev -y
sudo apt install libmpfr-dev -y
sudo apt install libmpfi-dev -y
sudo apt install flatbuffers-compiler -y
sudo apt install libflatbuffers-dev -y
sudo apt install cloc -y
echo "Setup finished"