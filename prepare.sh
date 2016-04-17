#!/bin/sh
#
# Copyright 2016 by Philip N. Garner
#
# See the file COPYING for the licence associated with this software.
#
# Author(s):
#   Phil Garner, April 2016
#

# CMake finders
gitraw=https://raw.githubusercontent.com
if [ ! -e cmake/FindLibUBE.cmake ]
then
    curl -LO $gitraw/pgarner/libube/master/cmake/FindLibUBE.cmake
    mkdir -p cmake
    mv FindLibUBE.cmake cmake
fi
