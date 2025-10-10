# Set the minimum required version of CMake.
cmake_minimum_required(VERSION 3.20)

# Define the project. This should be done only once, at the top level.
project(Workbench VERSION 0.0.1 LANGUAGES CXX)

# --- Build Options ---
# Options to control which components to build
option(BUILD_CORE "Build the core library" ON)
option(BUILD_GUI "Build the GUI application" ON)
option(BUILD_TOOLS "Build command-line tools" ON)
option(BUILD_WEBUI "Build the web UI" OFF)

# Convenience options for common combinations
option(BUILD_ALL "Build all components" OFF)
option(BUILD_CLI_ONLY "Build only core and tools (no GUI)" OFF)
option(BUILD_GUI_ONLY "Build only core and GUI (no tools)" OFF)

# Handle convenience options
if(BUILD_ALL)
    set(BUILD_CORE ON)
    set(BUILD_GUI ON)
    set(BUILD_TOOLS ON)
    set(BUILD_WEBUI ON)
endif()

if(BUILD_CLI_ONLY)
    set(BUILD_CORE ON)
    set(BUILD_GUI OFF)
    set(BUILD_TOOLS ON)
    set(BUILD_WEBUI OFF)
endif()

if(BUILD_GUI_ONLY)
    set(BUILD_CORE ON)
    set(BUILD_GUI ON)
    set(BUILD_TOOLS OFF)
    set(BUILD_WEBUI OFF)
endif()

# --- Global Settings ---
# Set the C++ standard for the entire project.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force a specific compiler if needed (optional).
# set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
# set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")

# Print build configuration
message(STATUS "=== Workbench Build Configuration ===")
message(STATUS "BUILD_CORE: ${BUILD_CORE}")
message(STATUS "BUILD_GUI: ${BUILD_GUI}")
message(STATUS "BUILD_TOOLS: ${BUILD_TOOLS}")
message(STATUS "BUILD_WEBUI: ${BUILD_WEBUI}")
message(STATUS "=====================================")

# --- Include Sub-projects ---
# This is the core of the monorepo. CMake will descend into these
# directories and process their CMakeLists.txt files.
# The order matters if one component depends on another, so list
# libraries before the applications that use them.

if(BUILD_CORE)
    message(STATUS "Adding core library")
    add_subdirectory(apps/workbench/core)
endif()

if(BUILD_GUI)
    if(NOT BUILD_CORE)
        message(FATAL_ERROR "GUI requires CORE to be built. Enable BUILD_CORE or use BUILD_GUI_ONLY option.")
    endif()
    message(STATUS "Adding GUI application")
    add_subdirectory(apps/workbench/gui)
endif()

if(BUILD_TOOLS)
    if(NOT BUILD_CORE)
        message(FATAL_ERROR "Tools require CORE to be built. Enable BUILD_CORE or use BUILD_CLI_ONLY option.")
    endif()
    message(STATUS "Adding tools")
    add_subdirectory(apps/workbench/tools)
endif()

if(BUILD_WEBUI)
    message(STATUS "Adding web UI")
    add_subdirectory(apps/workbench/webui)
endif()