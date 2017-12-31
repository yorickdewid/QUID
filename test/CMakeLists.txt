# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quid_test VERSION 1.5 LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Sleep is only available with POSIX
#add_definitions(-D_POSIX_C_SOURCE=199309L)

add_executable(quid_test quid_test.c)
add_executable(chacha_test chacha_test.c)

# Define output directories
set_target_properties(quid_test
	PROPERTIES
	OUTPUT_NAME "quid_test"
	PROJECT_LABEL "QUID Unit Test"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Define output directories
set_target_properties(chacha_test
	PROPERTIES
	OUTPUT_NAME "chacha_test"
	PROJECT_LABEL "ChaCha Unit Test"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(quid_test quid_a)
target_link_libraries(chacha_test quid_a)
