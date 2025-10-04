# Set the minimum required version of CMake.
cmake_minimum_required(VERSION 3.20)

# Define the project. This should be done only once, at the top level.
project(Workbench VERSION 0.0.1 LANGUAGES CXX)

# --- Global Settings ---
# Set the C++ standard for the entire project.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force a specific compiler if needed (optional).
# set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
# set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")

# --- Include Sub-projects ---
# This is the core of the monorepo. CMake will descend into these
# directories and process their CMakeLists.txt files.
# The order matters if one component depends on another, so list
# libraries before the applications that use them.
add_subdirectory(apps/core)
add_subdirectory(apps/gui)