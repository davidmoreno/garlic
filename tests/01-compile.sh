#!/bin/sh

set -e

rm -rf build-debug 
mkdir build-debug 
cd build-debug 
cmake ..
time make 
cd ..
rm -rf build-debug 

exit 0
