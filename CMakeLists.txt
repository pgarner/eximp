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

find_package(LibUBE REQUIRED)
find_package(LibEXIF REQUIRED)
# find_package(PNG REQUIRED)
find_package(Boost COMPONENTS
  filesystem REQUIRED
  )
include_directories(
  ${LIBUBE_INCLUDE_DIRS}
  ${LIBEXIF_INCLUDE_DIRS}
#  ${PNG_INCLUDE_DIRS}
  )
set(TARGET_LIBS
  ${LIBUBE_LIBRARIES}
  ${LIBEXIF_LIBRARIES}
#  ${PNG_LIBRARIES}
  ${Boost_LIBRARIES}
  )

# Main executable
add_executable(eximp eximp.cpp
  exif.cpp
#  png.cpp
  )
target_link_libraries(eximp ${TARGET_LIBS})
install(
  TARGETS eximp
  RUNTIME DESTINATION bin
  )

# The exiv library
#add_library(exif-lib MODULE exif.cpp)
#set_target_properties(exif-lib
#  PROPERTIES OUTPUT_NAME "exif"
#  )
#target_link_libraries(exif-lib card)
#list(APPEND MODULE_TARGETS exif-lib)
