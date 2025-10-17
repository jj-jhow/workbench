# obj2usd - Standalone OBJ to USD Converter

A command-line tool for converting OBJ files to USD format using the Workbench Core library.

## Features

- Convert OBJ files to USD/USDA format
- Configurable up-axis (Y or Z)
- Automatic output file generation
- Built on top of Assimp and Pixar USD

## Dependencies

- **Pixar USD** - USD file format support
- **Assimp** - OBJ file parsing
- **FBX SDK** - For the workbench core library
- **Workbench Core** - The core conversion library

## Building

### Prerequisites

1. Build and install the workbench core library first:
   ```bash
   cd /path/to/workbench
   cmake -S . -B build
   cmake --build build
   cmake --install build --prefix=/path/to/workbench/install
   ```

2. Ensure you have USD, Assimp, and FBX SDK installed.

### Build the standalone tool

```bash
cd workbench/apps/tools/converters/obj2usd
mkdir build
cd build
cmake ..
cmake --build .
```

### Install

```bash
cmake --install . --prefix=/usr/local
```

## Usage

```bash
# Basic conversion
obj2usd input.obj

# Specify output file
obj2usd -o output.usda input.obj

# Set up-axis
obj2usd -u z -o output.usda input.obj

# Show help
obj2usd --help
```

## Command Line Options

- `INPUT` - Input OBJ file (required)
- `-o, --output OUTPUT` - Specify output USD file
- `-u, --up-axis AXIS` - Set up axis (y or z, default: y)
- `-h, --help` - Show help message

## Environment Setup

The tool requires certain libraries to be in your library path:

```bash
export LD_LIBRARY_PATH=/opt/fbx202037_fbxsdk_linux/lib/release:/path/to/assimp/lib:$LD_LIBRARY_PATH
```

## Examples

```bash
# Convert shapes.obj to shapes.usda with Y up-axis
obj2usd shapes.obj

# Convert with Z up-axis and custom output
obj2usd -u z -o my_model.usda input.obj
```