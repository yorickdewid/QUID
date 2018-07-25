# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quidpp CXX)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_custom_target(QUID++ SOURCES quidpp.h)
