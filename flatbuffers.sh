#!/bin/bash

flatc --cpp -o src/data/ src/data/data.fbs

flatc --ts -o web/data/ src/data/data.fbs