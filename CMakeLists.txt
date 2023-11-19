#
# SPDX-License-Identifier: Apache-2.0
#
# PicoGK ("peacock") is a compact software kernel for computational geometry,
# specifically for use in Computational Engineering Models (CEM).
#
# For more information, please visit https://picogk.org
#
# PicoGK is developed and maintained by LEAP 71 - © 2023 by LEAP 71
# https://leap71.com
#
# Computational Engineering will profoundly change our physical world in the
# years ahead. Thank you for being part of the journey.
#
# We have developed this library to be used widely, for both commercial and
# non-commercial projects alike. Therefore, we have released it under a 
# permissive open-source license.
#
# The foundation of PicoGK is a thin layer on top of the powerful open-source
# OpenVDB project, which in turn uses many other Free and Open Source Software
# libraries. We are grateful to be able to stand on the shoulders of giants.
#
# LEAP 71 licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with the
# License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, THE SOFTWARE IS
# PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
#
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.27.4)

# Define the version
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 1)
set(LIB_VERSION_SUBMINOR 0)
set(LIB_VERSION "${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_SUBMINOR}")

cmake_policy( SET CMP0091 NEW ) #enable MSVC_RUNTIME_LIBRARY property
cmake_policy( SET CMP0042 NEW ) #enable MACOSX_RPATH
set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" )

project(PicoGK VERSION ${LIB_VERSION})

# Set the C++ standard to C++20 or higher
set(CMAKE_CXX_STANDARD 20)

# Set the output directory for the generated libraries and executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(PICOGK_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

# Define the library name
set(LIB_NAME "picogk")

# Option to control submodule update and initialization
option(PICOGK_UPDATE_SUBMODULES "Update and initialize Git submodules" OFF)

if(PICOGK_UPDATE_SUBMODULES)
    message(STATUS "Updating and initializing Git submodules...")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

foreach( OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUT_CONFIG CMAKE_CONFIGURATION_TYPES )

if( WIN32 )
  set( BOOTSTRAP_PATH ${PICOGK_ROOT_DIR}/Install_Dependencies/vcpkg/installed/x64-windows/ )
  set( BOOTSTRAP_BIN ${BOOTSTRAP_PATH}/bin )
  list( APPEND CMAKE_PREFIX_PATH ${BOOTSTRAP_PATH} )
endif()

#without this, the object files are over 4GB and can't be packaged into a .lib
set( USE_EXPLICIT_INSTANTIATION OFF )

#use release TBB for both debug and release configs
set( TBB_DEBUG_SUFFIX "" )

add_subdirectory( ${PICOGK_ROOT_DIR}/openvdb EXCLUDE_FROM_ALL )

if( MSVC )
  set_property(TARGET openvdb_static PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

#include GLFW

set( GLFW_SOURCE_DIR ${PICOGK_ROOT_DIR}/GLFW )

include( FetchContent )
set( FETCHCONTENT_QUIET off )
set( FETCHCONTENT_BASE_DIR ${GLFW_SOURCE_DIR} )
set( repo_glfw https://github.com/glfw/glfw.git )

FetchContent_Declare( glfw
  GIT_REPOSITORY                      ${repo_glfw}
  GIT_TAG                             3.3.8
  GIT_PROGRESS                        TRUE
  FETCHCONTENT_TRY_FIND_PACKAGE_MODE  ON )

FetchContent_MakeAvailable( glfw )

# Add library target
add_library(${LIB_NAME} SHARED)

# Add a preprocessor definition for the debug build
target_compile_definitions(${LIB_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD>)

if ( APPLE )
    set_target_properties( ${LIB_NAME} PROPERTIES
        VERSION ${LIB_VERSION}
        SOVERSION "${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.0"
    )
endif()

# Add all .cpp and .c files recursively from the Source directory
file(GLOB_RECURSE SOURCES
    "Source/*.cpp"
    "Source/*.c"
)

# Add the sources to the library target
target_sources(${LIB_NAME}
    PRIVATE
    ${SOURCES}
)

# Include directories for the library target
target_include_directories(${LIB_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/API
)

# Add the API header files to the library target
file(GLOB API_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/API/*.h")
target_sources(${LIB_NAME} PRIVATE ${API_HEADERS})

# Add the header files from the CPP folder to the library target
file(GLOB CPP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")
target_sources(${LIB_NAME} PRIVATE ${CPP_HEADERS})

# Organize files into virtual folders using the source_group command
source_group("PicoGK API" FILES ${API_HEADERS})

source_group("PicoGK Classes" FILES ${CPP_HEADERS})

# Add compiler definitions for the library target
target_compile_definitions(${LIB_NAME} PRIVATE PICOGK_BUILD_LIBRARY)

# Set different compiler flags for different build configurations

if( MSVC )
    message("Adding bigobj")
    target_compile_options(${LIB_NAME} PRIVATE "/bigobj")
else ()

    target_compile_options(${LIB_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -fvisibility=hidden -fvisibility-inlines-hidden>
        $<$<CONFIG:Release>:-O3 -fvisibility=hidden -fvisibility-inlines-hidden>
)
endif()

target_link_libraries(${LIB_NAME} openvdb_static glfw )

set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")

# Set the output directory for the library target based on build configurations
set_target_properties(${LIB_NAME} PROPERTIES

LIBRARY_OUTPUT_DIRECTORY $<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/lib,${CMAKE_BINARY_DIR}/lib>)

foreach(definition IN LISTS compile_definitions)
    message("Compile Definition for ${LIB_NAME}: ${definition}")
endforeach()

# Set the Debug postfix for the library name
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "d")

# Add executable target for APITests
add_executable(APITests)

# Add the main.cpp file from APITests directory
target_sources(APITests PRIVATE APITests/main.cpp)

# Link the APITests executable with the library target
target_link_libraries(APITests PRIVATE ${LIB_NAME})

# Define a custom command to copy header files to Dist folder
add_custom_command(
    TARGET ${LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/API
        ${CMAKE_CURRENT_SOURCE_DIR}/Dist
    COMMENT "Copying header files to Dist folder"
)

# Define a custom command to copy library files to Dist folder
add_custom_command(
    TARGET ${LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_BINARY_DIR}/Lib
        ${CMAKE_CURRENT_SOURCE_DIR}/Dist
    COMMENT "Copying library files to Dist folder"
)

add_custom_command(
    TARGET ${LIB_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Creating build headers..."
    COMMAND ${CMAKE_COMMAND} 	-DAPIDIR=${CMAKE_CURRENT_SOURCE_DIR}/API 
    							-DLIB_VERSION=${LIB_VERSION}
    							-DLIB_NAME=${LIB_NAME}
    							-P ${CMAKE_CURRENT_SOURCE_DIR}/CMake_Utils/PicoGKBuildHeader.cmake
    COMMENT "Create build headers"
)



 

