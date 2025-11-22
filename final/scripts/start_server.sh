#!/bin/bash

mkdir -p build
cd build
cmake ..
make
cd ..

./final -h 0.0.0.0 -p 8080 -d www
