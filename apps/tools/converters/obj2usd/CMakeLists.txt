cmake_minimum_required(VERSION 3.20)

# Project for obj2usd standalone tool
project(obj2usd VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the path to your FBX SDK installation
set(FBX_SDK_PATH "/opt/fbx202037_fbxsdk_linux")
set(CMAKE_PREFIX_PATH "/home/jonathan/repos/assimp/install" ${CMAKE_PREFIX_PATH})

# Find required packages
find_package(pxr REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Find Assimp - try config first, then fallback to manual search
find_package(assimp CONFIG QUIET)

# Check if workbench_core is available as an installed library
# If not, we'll need to find it or build it
find_library(WORKBENCH_CORE_LIB 
    NAMES workbench_core libworkbench_core.a
    PATHS 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../build/apps/core
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../install/lib
        /usr/local/lib
        /usr/lib
    NO_DEFAULT_PATH
)

if(NOT WORKBENCH_CORE_LIB)
    message(FATAL_ERROR "workbench_core library not found. Please build the core library first.")
endif()

message(STATUS "Found workbench_core: ${WORKBENCH_CORE_LIB}")

# Find workbench_core headers
find_path(WORKBENCH_CORE_INCLUDE_DIR
    NAMES converters/ConverterFactory.h
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/src/public
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../install/include
        /usr/local/include
        /usr/include
    NO_DEFAULT_PATH
)

if(NOT WORKBENCH_CORE_INCLUDE_DIR)
    message(FATAL_ERROR "workbench_core headers not found.")
endif()

message(STATUS "Found workbench_core headers: ${WORKBENCH_CORE_INCLUDE_DIR}")

# Create the executable
add_executable(obj2usd main.cpp)

# Include directories
target_include_directories(obj2usd PRIVATE
    ${WORKBENCH_CORE_INCLUDE_DIR}
    ${PXR_INCLUDE_DIRS}
    ${FBX_SDK_PATH}/include
)

# Include the fbxsdk.cmake script to set up the FBX SDK targets
include(${FBX_SDK_PATH}/samples/fbxsdk.cmake)

# Link libraries
target_link_libraries(obj2usd PRIVATE
    ${WORKBENCH_CORE_LIB}
    usd
    usdGeom
    fbxsdk
    usdImagingGL
    assimp::assimp
)

# Installation
install(TARGETS obj2usd
    RUNTIME DESTINATION bin
)

# Print build information
message(STATUS "obj2usd standalone configuration:")
message(STATUS "  USD Include: ${PXR_INCLUDE_DIRS}")
message(STATUS "  Workbench Core Lib: ${WORKBENCH_CORE_LIB}")
message(STATUS "  Workbench Core Headers: ${WORKBENCH_CORE_INCLUDE_DIR}")
message(STATUS "  FBX SDK: ${FBX_SDK_PATH}")