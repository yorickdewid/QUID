# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quid VERSION 1.5 LANGUAGES C)

# In-source builds are disabled.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMAKE generation is not possible within the source directory!")
endif()

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# Build sources with additional warnings
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -std=c99-Wmissing-prototypes \
	-Wpointer-arith \
	-Wendif-labels \
	-Wmissing-format-attribute \
	-Wformat-security \
	-Wno-strict-aliasing")
endif()
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /sdl")
	set(CMAKE_C_STANDARD 99)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Direct output to certain directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_definitions(-DPACKAGE_NAME="${PROJECT_NAME}")
add_definitions(-DPACKAGE_VERSION="${PROJECT_VERSION}")
add_definitions(-DPACKAGE_="${PROJECT_NAME}")
add_definitions(-DVERSION="${PROJECT_VERSION}")

add_library(quid STATIC
	include/quid.h
	src/quid.c
	src/chacha.c
	src/chacha.h
)

# Define output directories
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME "quid"
	PROJECT_LABEL "Quenza Unique Identifier"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
