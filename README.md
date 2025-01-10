# Prince Rupert's Cube

## Setup

```bash
sudo apt update
#sudo apt install libeigen3-dev
sudo apt install libopencv-dev
```

## Build and Run

```bash
mkdir -p build
cd build
cmake ..
make
./rupert
```

## Line Count

```bash
`cloc . --exclude-dir=build,.idea`
```