# USD Mesh Optimizer

This directory contains mesh optimization tools for the Workbench project. The optimizer provides functionality to optimize USD mesh primitives for better performance and reduced file sizes.

## Overview

The optimizer includes several tools:

### MeshTriangulator
The `MeshTriangulator` class converts n-gon faces in USD mesh primitives to triangular faces.

### HiddenMeshRemover
The `HiddenMeshRemover` class identifies and removes mesh primitives that are not visible from any reasonable viewpoint, helping to reduce file size and improve performance.

## Features

### Mesh Triangulation
- **Stage-wide triangulation**: Process all meshes in a USD stage with a single call
- **Individual mesh processing**: Triangulate specific mesh primitives
- **Time-aware processing**: Support for animated geometry at specific time codes
- **Primvar preservation**: Automatically triangulate face-varying primvar data
- **Statistics tracking**: Detailed reporting of triangulation results
- **Configurable options**: Control triangulation behavior through options

### Hidden Mesh Optimization
- **Non-destructive approach**: Uses USD's `visibility=invisible` instead of removing meshes
- **Visibility analysis**: Test mesh visibility from multiple viewpoints
- **Camera integration**: Use existing camera viewpoints in the scene
- **Automatic viewpoint generation**: Generate viewpoints around the scene bounds
- **Occlusion testing**: Consider mesh-to-mesh occlusion
- **Conservative hiding**: Configurable aggressiveness levels
- **Instance preservation**: Optionally preserve instanced meshes
- **Dry run mode**: Analyze without making changes
- **USD compliant**: Follows USD's non-destructive editing philosophy

## Algorithms

### Triangulation Algorithm

### Triangulation Algorithm

The triangulator uses a **fan triangulation** approach:

- For a face with vertices `[v0, v1, v2, v3, v4]`, it creates triangles:
  - `[v0, v1, v2]`
  - `[v0, v2, v3]` 
  - `[v0, v3, v4]`

This approach is simple, efficient, and preserves the face orientation.

### Hidden Mesh Removal Algorithm

The hidden mesh remover uses a multi-viewpoint visibility testing approach:

1. **Viewpoint Generation**: 
   - Extract camera viewpoints from the scene
   - Generate additional viewpoints in a sphere around the scene bounds
   
2. **Visibility Testing**:
   - Test each mesh's visibility from all viewpoints
   - Sample points on mesh surfaces for accurate testing
   - Perform ray-mesh intersection tests for occlusion
   
3. **Conservative Removal**:
   - Only remove meshes that are hidden from ALL viewpoints
   - Consider occlusion threshold for partial visibility
   - Preserve instanced meshes when requested

## Usage

### C++ API

#### Mesh Triangulation

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

#### Hidden Mesh Removal

```cpp
#include "optimizer/HiddenMeshRemover.h"

// Create remover with options
workbench::optimizer::HiddenMeshRemover::RemovalOptions options;
options.verbose = true;
options.useExistingCameras = true;
options.generateViewpoints = true;
options.occlusionThreshold = 0.95f;

workbench::optimizer::HiddenMeshRemover remover(options);

// Remove hidden meshes
UsdStageRefPtr stage = UsdStage::Open("input.usd");
bool success = remover.removeHiddenMeshes(stage);

// Get statistics
const auto& stats = remover.getStats();
std::cout << "Removed " << stats.removedMeshes << " of " << stats.totalMeshes 
          << " meshes (" << stats.spaceSavedPercent << "% reduction)" << std::endl;

// Save result
stage->Export("output.usd");
```

### Command Line Tools

#### Mesh Triangulation

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

#### Hidden Mesh Optimization

The `remove_hidden_meshes` tool provides command-line access to the hidden mesh optimization functionality:

```bash
# Basic usage - sets visibility=invisible for hidden meshes
./remove_hidden_meshes input.usd

# Analyze without modifying visibility (dry run)
./remove_hidden_meshes --dry-run input.usd

# Specify output file
./remove_hidden_meshes input.usd optimized.usd

# Verbose output with custom settings
./remove_hidden_meshes -v --viewpoint-density 12 --occlusion-threshold 0.9 input.usd

# Aggressive removal (less conservative)
./remove_hidden_meshes --aggressive input.usd

# Modify file in-place
./remove_hidden_meshes --in-place input.usd
```

### Working with Optimized Files

The hidden mesh optimization sets the `visibility` attribute to `invisible` for occluded meshes. This follows USD's non-destructive editing philosophy:

- **Invisible meshes remain in the scene** but won't be rendered
- **Storage size is reduced** as invisible meshes can be excluded during file operations
- **Optimization is reversible** - visibility can be restored if needed
- **Downstream tools** can choose to process or skip invisible geometry

To restore visibility of all meshes:
```bash
# You can restore visibility using USD tools or write a simple script
# Example using usdview: File > Preferences > Show Invisible Prims
```

## Options

### TriangulationOptions

- `preserveOriginalPrimvars` (default: true): Whether to preserve and triangulate primvar data
- `inPlace` (default: false): Whether to modify meshes in-place or create new topology
- `verbose` (default: false): Enable detailed logging output

### RemovalOptions

- `useExistingCameras` (default: true): Use cameras defined in the scene for viewpoints
- `generateViewpoints` (default: true): Generate additional viewpoints around the scene
- `viewpointDensity` (default: 8.0): Number of viewpoints per axis when generating
- `conservativeRemoval` (default: true): Be conservative - only remove obviously hidden meshes
- `considerTransparency` (default: true): Consider transparent materials when determining visibility
- `preserveInstancedMeshes` (default: true): Don't remove meshes that are instanced multiple times
- `occlusionThreshold` (default: 0.95): Fraction of mesh that must be occluded to consider it hidden
- `verbose` (default: false): Enable detailed logging output

## Statistics

### Triangulation Statistics

The triangulator tracks and reports:

- `meshesProcessed`: Number of mesh primitives processed
- `facesTriangulated`: Number of faces that required triangulation (had > 3 vertices)
- `originalFaceCount`: Total number of faces before triangulation
- `finalFaceCount`: Total number of faces after triangulation

### Removal Statistics

The hidden mesh remover tracks and reports:

- `totalMeshes`: Total number of meshes in the scene
- `hiddenMeshes`: Number of meshes detected as hidden
- `removedMeshes`: Number of meshes actually removed
- `preservedMeshes`: Number of meshes preserved (e.g., instanced meshes)
- `viewpointsUsed`: Total number of viewpoints used for analysis
- `viewpointsGenerated`: Number of viewpoints automatically generated
- `spaceSavedPercent`: Percentage of meshes removed

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

### Triangulation Limitations

1. **Fan triangulation**: May not be optimal for highly concave polygons
2. **Primvar interpolation**: Complex primvar configurations may need manual verification
3. **Memory usage**: Large meshes are processed in memory, which may require significant RAM
4. **Animation**: Time-varying topology is not automatically detected

### Hidden Mesh Removal Limitations

1. **Simplified occlusion**: Uses bounding box intersection rather than precise triangle-ray intersection
2. **Static analysis**: Does not consider animated camera paths or moving geometry
3. **Transparency approximation**: Basic transparency consideration - complex material graphs not fully supported
4. **Instance detection**: Simplified instancing detection may miss complex USD composition patterns
5. **Viewpoint coverage**: Generated viewpoints may not cover all relevant viewing angles for complex scenes

## Future Enhancements

Potential improvements for future versions:

### Triangulation Enhancements
- **Ear clipping triangulation**: Better handling of concave polygons
- **Batch processing**: Process multiple files in a single operation
- **Mesh validation**: Pre-triangulation mesh quality checks
- **Custom triangulation strategies**: Pluggable triangulation algorithms

### Hidden Mesh Removal Enhancements
- **Precise ray-triangle intersection**: More accurate occlusion testing
- **Material-aware visibility**: Consider opacity, transparency, and complex material graphs
- **Temporal analysis**: Support for animated cameras and time-varying geometry
- **Importance-based removal**: Consider mesh size, detail level, and artistic importance
- **Interactive preview**: Visual feedback showing which meshes would be removed
- **Multi-threading**: Parallel processing of visibility tests
- **Advanced instancing**: Better detection of USD instancing patterns

## Examples

See the `resources/` directory for example USD files that can be used to test the triangulation functionality.