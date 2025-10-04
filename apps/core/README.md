# Workbench Core Library

The core conversion library for USD/FBX file format operations, providing a robust foundation for file format conversion tools.

## Features

- **Multi-format Support**: OBJ ↔ USD, FBX ↔ USD conversion
- **Factory Pattern**: Extensible converter system using factory pattern
- **USD Integration**: Full Pixar USD support with proper material handling
- **Assimp Backend**: Robust mesh parsing using Assimp library
- **Type-safe APIs**: Modern C++17 with filesystem support

## Dependencies

### Required
- **CMake** 3.20+
- **C++17** compatible **compiler**
- **Pixar USD** - Universal Scene Description
- **Assimp** - Asset Import Library
- **FBX SDK** - Autodesk FBX SDK (2020.3.7)
- **Qt5** - For GUI components (Core, Widgets, OpenGL)

### System Packages (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libgl1-mesa-dev libglu1-mesa-dev
sudo apt install qt5-default qtbase5-dev qttools5-dev
```

## Project Structure

```
apps/core/
├── CMakeLists.txt           # Build configuration
├── src/
│   ├── public/             # Public headers (API)
│   │   ├── converters/     # Converter interfaces and implementations
│   │   │   ├── IConverter.h
│   │   │   ├── ConverterFactory.h
│   │   │   ├── ObjToUsdConverter.h
│   │   │   ├── UsdToFbxConverter.h
│   │   │   ├── FbxToUsdConverter.h
│   │   │   └── UpAxis.h
│   │   ├── importers/      # Import utilities
│   │   └── StageManager.h  # USD stage management
│   └── private/            # Implementation files
│       ├── converters/     # Converter implementations
│       ├── importers/
│       └── StageManager.cpp
```

## Building

### Prerequisites Setup

1. **Install Pixar USD** (if not already installed):
   ```bash
   # Set USD installation path
   export PXR_INSTALL_PATH=/path/to/your/USD/install
   ```

2. **Install Assimp** (if not already installed):
   ```bash
   git clone https://github.com/assimp/assimp.git
   cd assimp
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/assimp/install
   make -j$(nproc) && make install
   ```

3. **Verify FBX SDK** is installed at `/opt/fbx202037_fbxsdk_linux`

### Build Steps

```bash
# 1. Navigate to workbench root
cd /path/to/workbench

# 2. Configure the build (from project root)
cmake -S . -B build

# 3. Build the core library
cmake --build build

# 4. Install to local directory
cmake --install build --prefix=/path/to/workbench/install
```

### Build Configuration Options

```bash
# Debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Release build (default)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Custom USD path
cmake -S . -B build -Dpxr_DIR=/custom/usd/path

# Custom install prefix
cmake --install build --prefix=/custom/install/path
```

## Installation

After building, install the library and headers:

```bash
cmake --install build --prefix=/path/to/install
```

This installs:
- **Library**: `libworkbench_core.a` → `lib/`
- **Headers**: Public API → `include/`
- **GUI App**: `workbench_gui` → `bin/`

## Usage

### Using the Library

```cpp
#include "converters/ConverterFactory.h"
#include "converters/UpAxis.h"

using namespace converters;

// Get converter for OBJ to USD
auto converter = ConverterFactory::Instance().GetConverterFor("input.obj", "usd");

// Set conversion options
ConverterOptions options;
options.upAxis = UpAxis::Y;

// Perform conversion
bool success = converter->Convert("input.obj", "output.usda", options);
```

### Environment Setup

Create a setup script for development:

```bash
#!/bin/bash
# File: setup_env.sh

# Core dependencies
export LD_LIBRARY_PATH=/opt/fbx202037_fbxsdk_linux/lib/release:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/assimp/install/lib:$LD_LIBRARY_PATH

# USD
export PXR_INSTALL_PATH=/path/to/USD/install
export LD_LIBRARY_PATH=$PXR_INSTALL_PATH/lib:$LD_LIBRARY_PATH

# Workbench
export PATH=/path/to/workbench/install/bin:$PATH

echo "Workbench development environment loaded!"
```

Usage:
```bash
source setup_env.sh
```

## Rebuilding After Changes

### Quick Rebuild (Incremental)
```bash
cmake --build build
cmake --install build --prefix=./install
```

### Clean Rebuild (Full)
```bash
rm -rf build
cmake -S . -B build
cmake --build build
cmake --install build --prefix=./install
```

### Rebuild Only Core Library
```bash
cmake --build build --target workbench_core
cmake --install build --prefix=./install --component workbench_core
```

## Development Workflow

### 1. Make Code Changes
Edit files in `apps/core/src/`

### 2. Rebuild
```bash
cmake --build build
```

### 3. Install (if needed by tools)
```bash
cmake --install build --prefix=./install
```

### 4. Test
```bash
# Test with GUI
./install/bin/workbench_gui

# Test with standalone tools (if available)
./path/to/tools/obj2usd test.obj
```

## Troubleshooting

### Common Issues

1. **USD not found**:
   ```bash
   export PXR_INSTALL_PATH=/correct/path/to/USD
   ```

2. **Assimp not found**:
   ```bash
   cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/assimp/install"
   ```

3. **FBX SDK not found**:
   - Verify installation at `/opt/fbx202037_fbxsdk_linux`
   - Or update `FBX_SDK_PATH` in CMakeLists.txt

4. **Runtime library errors**:
   ```bash
   export LD_LIBRARY_PATH=/opt/fbx202037_fbxsdk_linux/lib/release:/path/to/assimp/lib:$LD_LIBRARY_PATH
   ```

### Build Verbose Output
```bash
cmake --build build --verbose
```

### Check Dependencies
```bash
ldd ./build/apps/core/libworkbench_core.a  # (won't work for static lib)
ldd ./build/apps/gui/workbench_gui
```

## API Reference

### Main Classes

- **`ConverterFactory`**: Factory for creating converters
- **`IConverter`**: Base interface for all converters
- **`ObjToUsdConverter`**: OBJ to USD conversion
- **`UsdToFbxConverter`**: USD to FBX conversion
- **`FbxToUsdConverter`**: FBX to USD conversion
- **`UpAxis`**: Up-axis enumeration and utilities
- **`StageManager`**: USD stage management utilities

### Key Headers

```cpp
#include "converters/ConverterFactory.h"  // Main factory
#include "converters/IConverter.h"        // Base interface
#include "converters/UpAxis.h"           // Up-axis utilities
#include "StageManager.h"                // USD stage management
```

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]