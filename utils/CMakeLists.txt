# Copyright (c) 2018 Quenza Inc. All rights reserved.
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
project(quidutil VERSION 1.8 LANGUAGES C)

configure_file(../config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

# Sleep is only available with POSIX
add_definitions(-D_POSIX_C_SOURCE=199309L)

add_executable(quidutil
	win32_getopt.h
	quidutil.c
)

# Define output directories
set_target_properties(quidutil
	PROPERTIES
	OUTPUT_NAME "quid"
	PROJECT_LABEL "QUID Utility"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(quidutil quid_a)

install(TARGETS quidutil DESTINATION bin)

add_test(NAME quidutil_generate COMMAND quidutil -c 100000 -d 0)
