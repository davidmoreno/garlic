#!/bin/sh

rm -rf build-debug && mkdir build-debug && cd build-debug && cmake .. && time make && rm -rf build-debug && exit 0

exit 1
