#!/bin/sh
#
# Copyright 2015 by Idiap Research Institute, http://www.idiap.ch
#
# See the file COPYING for the licence associated with this software.
#
# Author(s):
#   Phil Garner, September 2015
#
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake

export CC="clang -w"
export CXX=clang++

export CPATH=~/local/include

cmake \
    -D CMAKE_BUILD_TYPE=minsizerel \
    -D CMAKE_INSTALL_PREFIX=~/local \
    ..
