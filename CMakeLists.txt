cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)
project(ppepper)
find_library(ONIONCPP_LIB NAMES onioncpp PATH ${LIBPATH})
SET(OTEMPLATE otemplate)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O2 --std=c++11 -D__DEBUG__")


add_subdirectory(src)
