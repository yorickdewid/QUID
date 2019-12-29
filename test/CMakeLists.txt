# Copyright (c) 2020 Quenza Inc. All rights reserved.
# Copyright (c) 2020 Blub Corp. All rights reserved.
#
# This file is part of the QUID project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be
# copied and/or distributed without the express of the author.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quid_test VERSION 1.7 LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/include)

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

# Add test
add_test(NAME quid_test COMMAND quid_test)
add_test(NAME chacha_test COMMAND chacha_test)
