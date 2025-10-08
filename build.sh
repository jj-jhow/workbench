#!/bin/bash

# Workbench Build Script
# This script provides convenient ways to build different components

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR="build"
CMAKE_BUILD_TYPE="Release"
CLEAN_BUILD=false
INSTALL=false

# Function to print usage
print_usage() {
    echo -e "${BLUE}Workbench Build Script${NC}"
    echo ""
    echo "Usage: $0 [OPTIONS] [COMPONENT]"
    echo ""
    echo "Components:"
    echo "  all       - Build all components (default)"
    echo "  core      - Build only core library"
    echo "  gui       - Build core + GUI application"
    echo "  tools     - Build core + command-line tools"
    echo "  cli       - Build core + tools (no GUI)"
    echo "  gui-only  - Build core + GUI (no tools)"
    echo ""
    echo "Options:"
    echo "  -c, --clean      Clean build (remove build directory first)"
    echo "  -d, --debug      Build in Debug mode (default: Release)"
    echo "  -i, --install    Install after building"
    echo "  -b, --build-dir  Specify build directory (default: build)"
    echo "  -h, --help       Show this help"
    echo ""
    echo "Examples:"
    echo "  $0 core              # Build only core library"
    echo "  $0 gui --debug       # Build GUI in debug mode"
    echo "  $0 tools --clean     # Clean build tools"
    echo "  $0 all --install     # Build everything and install"
}

# Function to run cmake configure
configure_cmake() {
    local component=$1
    
    echo -e "${YELLOW}Configuring CMake for: $component${NC}"
    
    # Base cmake command
    local cmake_cmd="cmake -S . -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
    
    # Add component-specific options
    case $component in
        "all")
            cmake_cmd="$cmake_cmd -DBUILD_ALL=ON"
            ;;
        "core")
            cmake_cmd="$cmake_cmd -DBUILD_CORE=ON -DBUILD_GUI=OFF -DBUILD_TOOLS=OFF"
            ;;
        "gui")
            cmake_cmd="$cmake_cmd -DBUILD_CORE=ON -DBUILD_GUI=ON -DBUILD_TOOLS=OFF"
            ;;
        "tools")
            cmake_cmd="$cmake_cmd -DBUILD_CORE=ON -DBUILD_GUI=OFF -DBUILD_TOOLS=ON"
            ;;
        "cli")
            cmake_cmd="$cmake_cmd -DBUILD_CLI_ONLY=ON"
            ;;
        "gui-only")
            cmake_cmd="$cmake_cmd -DBUILD_GUI_ONLY=ON"
            ;;
        *)
            echo -e "${RED}Unknown component: $component${NC}"
            print_usage
            exit 1
            ;;
    esac
    
    echo "Running: $cmake_cmd"
    eval $cmake_cmd
}

# Function to build
build_project() {
    echo -e "${YELLOW}Building project...${NC}"
    cmake --build $BUILD_DIR --parallel $(nproc)
}

# Function to install
install_project() {
    echo -e "${YELLOW}Installing project...${NC}"
    cmake --install $BUILD_DIR
}

# Parse command line arguments
COMPONENT="all"
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -d|--debug)
            CMAKE_BUILD_TYPE="Debug"
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        all|core|gui|tools|cli|gui-only)
            COMPONENT="$1"
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Main execution
echo -e "${BLUE}=== Workbench Build Script ===${NC}"
echo -e "Component: ${GREEN}$COMPONENT${NC}"
echo -e "Build Type: ${GREEN}$CMAKE_BUILD_TYPE${NC}"
echo -e "Build Directory: ${GREEN}$BUILD_DIR${NC}"

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf $BUILD_DIR
fi

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Configure, build, and optionally install
configure_cmake $COMPONENT
build_project

if [ "$INSTALL" = true ]; then
    install_project
fi

echo -e "${GREEN}Build completed successfully!${NC}"