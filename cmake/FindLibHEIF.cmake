#
# Written by cfind.sh
# Part of lube, https://github.com/pgarner/libube
# See also https://cmake.org/Wiki/CMake:How_To_Find_Libraries
#
# The following should end up defined: 
#  LIBHEIF_FOUND          - System has LibHEIF
#  LIBHEIF_INCLUDE_DIR    - The LibHEIF include directories
#  LIBHEIF_LIBRARIES      - The libraries needed to use LibHEIF
#  LIBHEIF_DEFINITIONS    - Compiler switches required for using LibHEIF
#
find_package(PkgConfig)
pkg_check_modules(PC_LIBHEIF QUIET libheif)

set(LIBHEIF_DEFINITIONS ${PC_LIBHEIF_CFLAGS_OTHER})

find_path(
  LIBHEIF_INCLUDE_DIR heif.h
  HINTS ${PC_LIBHEIF_INCLUDEDIR} ${PC_LIBHEIF_INCLUDE_DIRS}
  PATH_SUFFIXES libheif
)

find_library(
  LIBHEIF_LIBRARY heif  # Without the lib- prefix
  HINTS ${PC_LIBHEIF_LIBDIR} ${PC_LIBHEIF_LIBRARY_DIRS}
)

set(LIBHEIF_LIBRARIES ${LIBHEIF_LIBRARY})  # Can add ${CMAKE_DL_LIBS}
set(LIBHEIF_INCLUDE_DIRS ${LIBHEIF_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LibHEIF DEFAULT_MSG
  LIBHEIF_LIBRARY LIBHEIF_INCLUDE_DIR
)

mark_as_advanced(LIBHEIF_INCLUDE_DIR LIBHEIF_LIBRARY)
