# Prince Rupert's Cube

## Setup

```bash
sudo apt update
#sudo apt install libeigen3-dev
#sudo apt install libopencv-dev
#sudo apt-get install libgmp-dev
#sudo apt-get install libmpfr-dev
#sudo apt-get install libmpfi-dev
```

## Build and Run

```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
cd ..
build/rupert
```

## Line Count

```bash
cloc . --exclude-dir=build,.idea
```