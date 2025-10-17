# Workbench Author Library

The author library provides USD authoring and editing capabilities for the Workbench application.

## Features

This library will contain tools and utilities for:

- **Primitive Creation**: Tools for creating USD primitives (meshes, lights, cameras, etc.)
- **Material Authoring**: Material and shading network creation and editing
- **Attribute Editing**: Generic attribute manipulation and validation
- **Scene Composition**: Tools for scene graph manipulation and composition arcs
- **Animation Authoring**: Keyframe and time-sample authoring utilities

## Architecture

The library is organized into several key components:

- `PrimCreator` - Factory for creating various USD primitive types
- `MaterialAuthor` - Material and shader authoring tools
- `AttributeEditor` - Generic attribute editing and validation
- `SceneComposer` - Scene composition and referencing tools
- `AnimationAuthor` - Animation and time-sample authoring

## Usage

```cpp
#include "PrimCreator.h"
#include "MaterialAuthor.h"

// Create a new mesh primitive
workbench::author::PrimCreator creator;
auto meshPrim = creator.createMesh(stage, "/World/MyMesh");

// Author a material
workbench::author::MaterialAuthor materialAuthor;
auto material = materialAuthor.createBasicMaterial(stage, "/Materials/MyMaterial");
```

## Dependencies

- USD (Pixar Universal Scene Description)
- Workbench Core Library