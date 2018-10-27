# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

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

string(TIMESTAMP PROJECT_COMPILE_YEAR "%Y")
set(PROJECT_BUGREPORT "yorick17 at outlook dot com" CACHE STRING "Address to report bugs")
set(PROJECT_URL "https://github.com/yorickdewid/QUID" CACHE STRING "Project website")
set(PROJECT_AUTHOR "Blub Corp." CACHE STRING "Project author")

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Direct output to certain directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})

set(PROJECT_SOURCES
	include/quid.h
	src/quid.c
	src/chacha.c
	src/chacha.h
)

add_library(quid_a STATIC ${PROJECT_SOURCES})
add_library(quid_lib SHARED ${PROJECT_SOURCES})

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

enable_testing()

add_subdirectory(utils)
add_subdirectory(test)
add_subdirectory(quidpp)
