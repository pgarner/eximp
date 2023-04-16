#
# Copyright 2014 by Philip N. Garner
#
# See the file COPYING for the licence associated with this software.
#
# Author(s):
#   Phil Garner, September 2014
#

cmake_minimum_required(VERSION 2.8)

project(eximp)
set(VERSION 0.1)

set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(lube REQUIRED)
find_package(LibEXIF REQUIRED)
find_package(PNG REQUIRED)
find_package(Boost COMPONENTS
  filesystem REQUIRED
  )

set(SOURCES
  eximp.cpp
  exif.cpp
  png.cpp
  )

include_directories(
  ${LIBEXIF_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  )
set(TARGET_LIBS
  lube::lube-shared
  ${LIBEXIF_LIBRARIES}
  ${PNG_LIBRARIES}
  ${Boost_LIBRARIES}
  )

find_package(LibHEIF)
if (LIBHEIF_FOUND)
  message(STATUS "Including LibHEIF support")
  add_definitions(-DHAVE_HEIF)
  include_directories(${LIBHEIF_INCLUDE_DIRS})
  list(APPEND TARGET_LIBS ${LIBHEIF_LIBRARIES})
  list(APPEND SOURCES heif.cpp)
endif()

find_package(LibAVFormat)
if (LIBAVFORMAT_FOUND)
  message(STATUS "Including LibAVFormat support")
  add_definitions(-DHAVE_AVFORMAT)
  include_directories(${LIBAVFORMAT_INCLUDE_DIRS})
  list(APPEND TARGET_LIBS ${LIBAVFORMAT_LIBRARIES})
  list(APPEND SOURCES avformat.cpp)
endif()

# Main executable
add_executable(eximp ${SOURCES})
target_link_libraries(eximp ${TARGET_LIBS})

#
# LibAVFormat has lots (some stupid number) of deps, so I messed with using
# dedicated MP4 libs.  In the end it seems too much trouble.  Below are some
# lines that may be helpful if that gets resurrected.
#

# find_package(LibMP4v2)
#  ${ISOBM_INCLUDE_DIRS}
#  ${LIBMP4V2_INCLUDE_DIRS}
#  ${LIBMP4V2_LIBRARIES}
#  ${ISOBM_LIBRARIES}
#  isobm.cpp
#  mp4v2.cpp
# add_dependencies(eximp ISOBMFF)

#include(ExternalProject)
#find_package(Git REQUIRED)
#ExternalProject_Add(ISOBMFF
#  GIT_REPOSITORY    https://github.com/pgarner/ISOBMFF
#  GIT_TAG           devel
#  BUILD_IN_SOURCE   true
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND     ${CMAKE_MAKE_PROGRAM}
#  INSTALL_COMMAND   ""
#)
#set(ISOBM_ROOT "${CMAKE_CURRENT_BINARY_DIR}/ISOBMFF-prefix/src/ISOBMFF")
#set(ISOBM_INCLUDE_DIRS
#  "${ISOBM_ROOT}/ISOBMFF/include"
#  "${ISOBM_ROOT}/Submodules/PIMPL/PIMPL/include"
#  )
#set(ISOBM_LIBRARIES "${ISOBM_ROOT}/Build/Debug/Products/x86_64/libISOBMFF.a")

# This only works after the ISOBMFF external is at least downloaded
#add_executable(dump ${ISOBM_ROOT}/ISOBMFF-Dump/main.cpp)
#target_link_libraries(dump ${TARGET_LIBS})

# The exiv library
#add_library(exif-lib MODULE exif.cpp)
#set_target_properties(exif-lib
#  PROPERTIES OUTPUT_NAME "exif"
#  )
#target_link_libraries(exif-lib card)
#list(APPEND MODULE_TARGETS exif-lib)

install(
  TARGETS eximp
  RUNTIME DESTINATION bin
  )
