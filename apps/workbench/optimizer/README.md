# Mesh Triangulation Optimizer

This directory contains the mesh triangulation optimizer for the Workbench project. The optimizer provides functionality to convert n-gon faces in USD mesh primitives to triangular faces.

## Overview

The `MeshTriangulator` class is designed to process USD stages and convert all polygonal faces with more than 3 vertices into triangular faces. This is particularly useful for:

- Preparing assets for rendering engines that require triangulated geometry
- Optimizing mesh data for GPU processing
- Ensuring consistent topology across different USD assets

## Features

- **Stage-wide triangulation**: Process all meshes in a USD stage with a single call
- **Individual mesh processing**: Triangulate specific mesh primitives
- **Time-aware processing**: Support for animated geometry at specific time codes
- **Primvar preservation**: Automatically triangulate face-varying primvar data
- **Statistics tracking**: Detailed reporting of triangulation results
- **Configurable options**: Control triangulation behavior through options

## Algorithm

The triangulator uses a **fan triangulation** approach:

- For a face with vertices `[v0, v1, v2, v3, v4]`, it creates triangles:
  - `[v0, v1, v2]`
  - `[v0, v2, v3]` 
  - `[v0, v3, v4]`

This approach is simple, efficient, and preserves the face orientation. While not optimal for all cases (concave polygons may produce degenerate triangles), it works well for most common mesh topologies.

## Usage

### C++ API

```cpp
#include "optimizer/MeshTriangulator.h"

// Create triangulator with options
workbench::optimizer::MeshTriangulator::TriangulationOptions options;
options.verbose = true;
options.preserveOriginalPrimvars = true;

workbench::optimizer::MeshTriangulator triangulator(options);

// Triangulate entire stage
UsdStageRefPtr stage = UsdStage::Open("input.usd");
bool success = triangulator.triangulateStage(stage);

// Get statistics
const auto& stats = triangulator.getStats();
std::cout << "Processed " << stats.meshesProcessed << " meshes" << std::endl;

// Save result
stage->Export("output.usd");
```

### Command Line Tool

The `triangulate_meshes` tool provides command-line access to the triangulation functionality:

```bash
# Basic usage
./triangulate_meshes input.usd

# Specify output file
./triangulate_meshes input.usd output.usd

# Verbose output
./triangulate_meshes -v input.usd

# Modify file in-place
./triangulate_meshes --in-place input.usd

# Don't preserve primvar data
./triangulate_meshes --no-primvars input.usd
```

## Options

### TriangulationOptions

- `preserveOriginalPrimvars` (default: true): Whether to preserve and triangulate primvar data
- `inPlace` (default: false): Whether to modify meshes in-place or create new topology
- `verbose` (default: false): Enable detailed logging output

## Statistics

The triangulator tracks and reports:

- `meshesProcessed`: Number of mesh primitives processed
- `facesTriangulated`: Number of faces that required triangulation (had > 3 vertices)
- `originalFaceCount`: Total number of faces before triangulation
- `finalFaceCount`: Total number of faces after triangulation

## Primvar Handling

Face-varying primvars are automatically triangulated to match the new face topology. The triangulator:

1. Identifies face-varying primvars
2. Expands indexed primvar data using fan triangulation
3. Preserves interpolation settings and data types

For complex primvar configurations, manual verification may be required.

## Building

The optimizer is built as part of the main Workbench project:

```bash
# Build all components including optimizer
cmake -S . -B build -DBUILD_ALL=ON
cmake --build build

# Build only core and optimizer
cmake -S . -B build -DBUILD_CORE=ON -DBUILD_OPTIMIZER=ON
cmake --build build
```

## Dependencies

- Pixar USD (Universal Scene Description)
- Workbench Core library
- Standard C++20 compiler

## Limitations

1. **Fan triangulation**: May not be optimal for highly concave polygons
2. **Primvar interpolation**: Complex primvar configurations may need manual verification
3. **Memory usage**: Large meshes are processed in memory, which may require significant RAM
4. **Animation**: Time-varying topology is not automatically detected

## Future Enhancements

Potential improvements for future versions:

- **Ear clipping triangulation**: Better handling of concave polygons
- **Batch processing**: Process multiple files in a single operation
- **Mesh validation**: Pre-triangulation mesh quality checks
- **Custom triangulation strategies**: Pluggable triangulation algorithms
- **Multi-threading**: Parallel processing of large stages

## Examples

See the `resources/` directory for example USD files that can be used to test the triangulation functionality.