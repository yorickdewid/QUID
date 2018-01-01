# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quidpp CXX)

include_directories(${CMAKE_SOURCE_DIR}/include)

#add_executable(quid_test quid_test.c)

add_custom_target(QUID++ SOURCES ${CMAKE_SOURCE_DIR}/quidpp/quidpp.h)

# Define output directories
#set_target_properties(quid_test
#	PROPERTIES
#	OUTPUT_NAME "quid_test"
#	PROJECT_LABEL "QUID Unit Test"
#	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#)

#target_link_libraries(chacha_test quid_a)
