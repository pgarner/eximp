#!/bin/zsh
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

opts=(
    -D CMAKE_BUILD_TYPE=relwithdebinfo
    -D CMAKE_INSTALL_PREFIX=~/local
)
cmake $opts ..
