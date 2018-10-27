# Copyright (c) 2018 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the QUID project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be
# copied and/or distributed without the express of the author.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quid VERSION 1.7 LANGUAGES C)

# In-source builds are disabled.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMAKE generation is not possible within the source directory!")
endif()

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# Build sources with additional warnings
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -std=c11 \
	-Wpointer-arith \
	-Wendif-labels \
	-Wmissing-format-attribute \
	-Wformat-security \
	-Wno-strict-aliasing")
endif()
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /sdl")
	set(CMAKE_C_STANDARD 11)
endif()

string(TIMESTAMP QUID_COMPILE_YEAR "%Y")
set(QUID_BUGREPORT "yorick17 at outlook dot com" CACHE STRING "Address to report bugs")
set(QUID_URL "https://github.com/yorickdewid/QUID" CACHE STRING "Project website")
set(QUID_AUTHOR "Blub Corp." CACHE STRING "Project author")

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Direct output to certain directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

set(QUID_SOURCES
	include/quid.h
	src/quid.c
	src/chacha.c
	src/chacha.h
)

add_library(quid_a STATIC ${QUID_SOURCES})
add_library(quid_lib SHARED ${QUID_SOURCES})

# Define output directories
set_target_properties(quid_a
	PROPERTIES
	OUTPUT_NAME "quid_a"
	PROJECT_LABEL "QUID Static Library"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Define output directories
set_target_properties(quid_lib
	PROPERTIES
	OUTPUT_NAME "quid"
	PROJECT_LABEL "QUID Dynamic Library"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(FILES include/quid.h DESTINATION include)
install(TARGETS quid_lib quid_a
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

if(NOT CPack_CMake_INCLUDED)
	set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
	set(CPACK_PACKAGE_VENDOR ${QUID_AUTHOR})
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})
	set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

	include(CPack)
endif()

enable_testing()

add_subdirectory(utils)
add_subdirectory(test)
