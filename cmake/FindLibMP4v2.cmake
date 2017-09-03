#
# Written by cfind.sh
# Part of lube, https://github.com/pgarner/libube
# See also https://cmake.org/Wiki/CMake:How_To_Find_Libraries
#
# The following should end up defined: 
#  LIBMP4V2_FOUND          - System has LibMP4v2
#  LIBMP4V2_INCLUDE_DIR    - The LibMP4v2 include directories
#  LIBMP4V2_LIBRARIES      - The libraries needed to use LibMP4v2
#  LIBMP4V2_DEFINITIONS    - Compiler switches required for using LibMP4v2
#
find_package(PkgConfig)
pkg_check_modules(PC_LIBMP4V2 QUIET libmp4v2)

set(LIBMP4V2_DEFINITIONS ${PC_LIBMP4V2_CFLAGS_OTHER})

find_path(
  LIBMP4V2_INCLUDE_DIR mp4v2/mp4v2.h
  HINTS ${PC_LIBMP4V2_INCLUDEDIR} ${PC_LIBMP4V2_INCLUDE_DIRS}
)

find_library(
  LIBMP4V2_LIBRARY mp4v2  # Without the lib- prefix
  HINTS ${PC_LIBMP4V2_LIBDIR} ${PC_LIBMP4V2_LIBRARY_DIRS}
)

set(LIBMP4V2_LIBRARIES ${LIBMP4V2_LIBRARY})  # Can add ${CMAKE_DL_LIBS}
set(LIBMP4V2_INCLUDE_DIRS ${LIBMP4V2_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LibMP4v2 DEFAULT_MSG
  LIBMP4V2_LIBRARY LIBMP4V2_INCLUDE_DIR
)

mark_as_advanced(LIBMP4V2_INCLUDE_DIR LIBMP4V2_LIBRARY)
