# obj2usd converter tool
# This can be built as part of the main workbench project or standalone

# Only set project and standards if this is being built standalone
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.20)
    project(obj2usd VERSION 1.0.0 LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(STANDALONE_BUILD TRUE)
    
    # Set CMake policies for standalone builds
    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0144 NEW)  # Use UPPER_CASE_ROOT variables
    endif()
else()
    set(STANDALONE_BUILD FALSE)
endif()

# Define the path to your FBX SDK installation
# Allow users to specify FBX SDK location via environment variable or CMake variable
if(DEFINED ENV{FBX_SDK_PATH})
    set(FBX_SDK_PATH "$ENV{FBX_SDK_PATH}")
elseif(NOT FBX_SDK_PATH)
    set(FBX_SDK_PATH "/opt/fbx202037_fbxsdk_linux")  # Default fallback
endif()

# Allow users to specify Assimp location via environment variable or CMake variable
if(DEFINED ENV{ASSIMP_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{ASSIMP_ROOT}")
elseif(ASSIMP_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${ASSIMP_ROOT}")
endif()

# Find required packages
find_package(pxr REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Find Assimp - try multiple methods for maximum compatibility
find_package(assimp CONFIG QUIET)
if(NOT assimp_FOUND)
    # Fallback to PkgConfig
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASSIMP QUIET assimp)
    endif()
    
    # Last resort: manual search
    if(NOT ASSIMP_FOUND)
        find_path(ASSIMP_INCLUDE_DIR assimp/Importer.hpp
            PATHS
                /usr/include
                /usr/local/include
                /opt/local/include
        )
        find_library(ASSIMP_LIBRARY
            NAMES assimp libassimp
            PATHS
                /usr/lib
                /usr/local/lib
                /opt/local/lib
        )
        if(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
            set(ASSIMP_FOUND TRUE)
            add_library(assimp::assimp UNKNOWN IMPORTED)
            set_target_properties(assimp::assimp PROPERTIES
                IMPORTED_LOCATION "${ASSIMP_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIR}"
            )
        endif()
    endif()
endif()

if(NOT assimp_FOUND AND NOT ASSIMP_FOUND)
    message(FATAL_ERROR "Assimp not found. Please install assimp or set ASSIMP_ROOT environment variable.")
endif()

if(STANDALONE_BUILD)
    # For standalone builds, find the pre-built workbench_core library
    find_library(WORKBENCH_CORE_LIB 
        NAMES workbench_core libworkbench_core.a
        PATHS 
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../build/workbench/libs/core
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../../install/lib
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
        NO_DEFAULT_PATH
    )

    if(NOT WORKBENCH_CORE_INCLUDE_DIR)
        message(FATAL_ERROR "workbench_core headers not found.")
    endif()

    message(STATUS "Found workbench_core headers: ${WORKBENCH_CORE_INCLUDE_DIR}")
endif()

# Create the executable
add_executable(obj2usd main.cpp)

# Include directories
if(STANDALONE_BUILD)
    target_include_directories(obj2usd PUBLIC
        ${WORKBENCH_CORE_INCLUDE_DIR}
        ${PXR_INCLUDE_DIRS}
        ${FBX_SDK_PATH}/include
    )
else()
    # For integrated builds, workbench_core is a target
    target_include_directories(obj2usd PUBLIC
        ${PXR_INCLUDE_DIRS}
        ${FBX_SDK_PATH}/include
    )
endif()

# Include the fbxsdk.cmake script to set up the FBX SDK targets
# Only include if not already included to avoid duplicate targets
if(NOT TARGET fbxsdk)
    include(${FBX_SDK_PATH}/samples/fbxsdk.cmake)
endif()

# Link libraries
if(STANDALONE_BUILD)
    target_link_libraries(obj2usd PUBLIC
        ${WORKBENCH_CORE_LIB}
        usd
        usdGeom
        fbxsdk
        usdImagingGL
        tf
        assimp::assimp
    )
else()
    # For integrated builds, link to the workbench_core target
    target_link_libraries(obj2usd PUBLIC
        workbench_core
        usd
        usdGeom
        fbxsdk
        usdImagingGL
        tf
        assimp::assimp
    )
endif()

# Installation
install(TARGETS obj2usd
    RUNTIME DESTINATION bin
)

# Print build information
if(STANDALONE_BUILD)
    message(STATUS "obj2usd standalone configuration:")
    message(STATUS "  USD Include: ${PXR_INCLUDE_DIRS}")
    message(STATUS "  Workbench Core Lib: ${WORKBENCH_CORE_LIB}")
    message(STATUS "  Workbench Core Headers: ${WORKBENCH_CORE_INCLUDE_DIR}")
    message(STATUS "  FBX SDK: ${FBX_SDK_PATH}")
else()
    message(STATUS "obj2usd integrated configuration:")
    message(STATUS "  USD Include: ${PXR_INCLUDE_DIRS}")
    message(STATUS "  Workbench Core: target workbench_core")
    message(STATUS "  FBX SDK: ${FBX_SDK_PATH}")
endif()