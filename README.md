# Prince Rupert's Cube

## Setup

```bash
sudo apt update
sudo apt install libopencv-dev
sudo apt install libboost-all-dev
sudo apt install libgmp-dev
sudo apt install libmpfr-dev
sudo apt install libmpfi-dev
```

## Build and Run

```bash
./build_and_run.sh
```
Or separately (e.g. for building once and running multiple times):
```bash
./build.sh
./run.sh
```

## Utils

### Line Count

```bash
cloc . --exclude-dir=build,.idea
```