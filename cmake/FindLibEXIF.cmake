#
# Copyright 2013 by Idiap Research Institute, http://www.idiap.ch
#
# See the file COPYING for the licence associated with this software.
#
# Author(s):
#   Phil Garner, December 2013
#

#
# Try to find Libexif
# Once done this will define
#  LIBEXIF_FOUND          - System has libexif
#  LIBEXIF_INCLUDE_DIR    - The libexif include directory
#  LIBEXIF_LIBRARIES      - The libraries needed to use Libexif
#  LIBEXIF_DEFINITIONS    - Compiler switches required for using libexif
#  LIBEXIF_VERSION_STRING - the version of libexif found
#

find_package(PkgConfig)
pkg_check_modules(PC_LIBEXIF QUIET libexif)

set(LIBEXIF_DEFINITIONS ${PC_LIBEXIF_CFLAGS_OTHER})
set(LIBEXIF_VERSION_STRING ${PC_LIBEXIF_VERSION})

find_path(
  LIBEXIF_INCLUDE_DIR exif-data.h
  HINTS ${PC_LIBEXIF_INCLUDEDIR} ${PC_LIBEXIF_INCLUDE_DIRS}
  PATH_SUFFIXES libexif
)

find_library(
  LIBEXIF_LIBRARY exif
  HINTS ${PC_LIBEXIF_LIBDIR} ${PC_LIBEXIF_LIBRARY_DIRS}
)

set(LIBEXIF_LIBRARIES ${LIBEXIF_LIBRARY} ${CMAKE_DL_LIBS})
set(LIBEXIF_INCLUDE_DIRS ${LIBEXIF_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LibEXIF
  REQUIRED_VARS LIBEXIF_LIBRARY LIBEXIF_INCLUDE_DIR
  VERSION_VAR LIBEXIF_VERSION_STRING
)

mark_as_advanced(LIBEXIF_INCLUDE_DIR LIBEXIF_LIBRARY)
