#
# Written by cfind.sh
# Part of lube, https://github.com/pgarner/libube
# See also https://cmake.org/Wiki/CMake:How_To_Find_Libraries
#
# The following should end up defined: 
#  LIBAVFORMAT_FOUND          - System has LibAVFormat
#  LIBAVFORMAT_INCLUDE_DIR    - The LibAVFormat include directories
#  LIBAVFORMAT_LIBRARIES      - The libraries needed to use LibAVFormat
#  LIBAVFORMAT_DEFINITIONS    - Compiler switches required for using LibAVFormat
#
find_package(PkgConfig)
pkg_check_modules(PC_LIBAVFORMAT QUIET libavformat)

set(LIBAVFORMAT_DEFINITIONS ${PC_LIBAVFORMAT_CFLAGS_OTHER})

find_path(
  LIBAVFORMAT_INCLUDE_DIR libavformat/avformat.h
  HINTS ${PC_LIBAVFORMAT_INCLUDEDIR} ${PC_LIBAVFORMAT_INCLUDE_DIRS}
  PATH_SUFFIXES libavformat
)

find_library(
  LIBAVFORMAT_LIBRARY avformat
  HINTS ${PC_LIBAVFORMAT_LIBDIR} ${PC_LIBAVFORMAT_LIBRARY_DIRS}
)

set(LIBAVFORMAT_LIBRARIES ${LIBAVFORMAT_LIBRARY} -lavutil)
set(LIBAVFORMAT_INCLUDE_DIRS ${LIBAVFORMAT_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  LibAVFormat DEFAULT_MSG
  LIBAVFORMAT_LIBRARY LIBAVFORMAT_INCLUDE_DIR
)

mark_as_advanced(LIBAVFORMAT_INCLUDE_DIR LIBAVFORMAT_LIBRARY)
