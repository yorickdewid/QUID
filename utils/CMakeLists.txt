# Copyright (c) 2012-2018, Yorick de Wid <yorick17 at outlook dot com>
# All rights reserved.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Set project info
project(quidutil VERSION 1.5 LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/include)

#add_definitions(-DPACKAGE_TARNAME="${PROJECT_NAME}")
#add_definitions(-DPACKAGE_NAME="${PROJECT_NAME}")
#add_definitions(-DPACKAGE_VERSION="${PROJECT_VERSION}")
#add_definitions(-DPACKAGE_STRING="${PROJECT_NAME} ${PROJECT_VERSION}")
#add_definitions(-DPACKAGE_BUGREPORT="yorick17@outlook.com")
#add_definitions(-DPACKAGE_URL="")
#add_definitions(-DPACKAGE="${PROJECT_NAME}")
#add_definitions(-DVERSION="${PROJECT_VERSION}")

# Sleep is only available with POSIX
add_definitions(-D_POSIX_C_SOURCE=199309L)

set(PROJECT_SOURCES
	quidutil.c
)

add_executable(quidutil ${PROJECT_SOURCES})

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
