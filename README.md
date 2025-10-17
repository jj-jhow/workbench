# Workbench

A modular C++ application suite using Pixar's OpenUSD (pxr) library for USD asset pipeline tools and GUI applications.

## Components

This monorepo is organized using a domain-first approach with components that can be built independently or together:

### Workbench Domain
- **Core Library** (`workbench/libs/core/`): Shared functionality, converters, and USD utilities
- **Optimizer Library** (`workbench/libs/optimizer/`): Mesh optimization algorithms and USD processing
- **GUI Application** (`workbench/apps/gui/`): Qt-based GUI for USD scene viewing and editing
- **Command-Line Tools** (`workbench/apps/tools/`): CLI utilities for USD conversion and processing
- **Web UI** (`workbench/apps/webui/`): Web-based interface (optional)

## Requirements
- CMake >= 3.20
- C++20 or newer
- OpenUSD (pxr) - set PXR_INSTALL_PATH environment variable
- Qt5 (for GUI components)
- FBX SDK (for FBX conversion tools)
- Assimp (for additional format support)

### Dependency Setup

#### Quick Setup (Recommended)
1. Copy the sample configuration file:
   ```bash
   cp config.sample.sh config.sh
   ```
2. Edit `config.sh` with your installation paths
3. Source the configuration:
   ```bash
   source config.sh
   ```

#### Manual Setup

##### OpenUSD
Set the environment variable pointing to your USD installation:
```bash
export PXR_INSTALL_PATH=/path/to/your/usd/installation
```

##### Assimp
The build system will automatically find Assimp if it's installed system-wide. For custom installations:
```bash
# Option 1: Environment variable
export ASSIMP_ROOT=/path/to/your/assimp/installation

# Option 2: CMake variable
cmake -DASSIMP_ROOT=/path/to/your/assimp/installation ...
```

##### Installing Dependencies on Ubuntu/Debian
```bash
# System-wide Assimp (recommended)
sudo apt install libassimp-dev

# Qt5 for GUI
sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools

# Build tools
sudo apt install cmake build-essential
```

## Building

### Quick Start
Use the provided build script for convenient builds:

```bash
# Build everything
./build.sh all

# Build only specific components
./build.sh core      # Core library only
./build.sh gui       # Core + GUI application
./build.sh tools     # Core + command-line tools
./build.sh cli       # Core + tools (no GUI)
./build.sh gui-only  # Core + GUI (no tools)

# Additional options
./build.sh all --clean     # Clean build
./build.sh all --debug     # Debug build
./build.sh all --install   # Build and install
```

### Manual CMake Build
You can also use CMake directly with build options:

```bash
# Configure with specific components
cmake -S . -B build -DBUILD_CORE=ON -DBUILD_GUI=OFF -DBUILD_TOOLS=ON

# Or use convenience options
cmake -S . -B build -DBUILD_ALL=ON
cmake -S . -B build -DBUILD_CLI_ONLY=ON
cmake -S . -B build -DBUILD_GUI_ONLY=ON

# Build
cmake --build build --parallel $(nproc)

# Install (optional)
cmake --install build
```

### VS Code Integration
The project includes VS Code tasks for easy building:

- `Ctrl+Shift+P` → "Tasks: Run Task"
- Choose from available build tasks:
  - "Build All Components" (default)
  - "Build Core Only"
  - "Build GUI Application"
  - "Build Command Line Tools"
  - "Build CLI Only (Core + Tools)"
  - "Build GUI Only (Core + GUI)"
  - "Clean Build All"
  - "Debug Build All"

### CMake Build Options

| Option           | Description                        | Default |
| ---------------- | ---------------------------------- | ------- |
| `BUILD_CORE`     | Build the core library             | ON      |
| `BUILD_GUI`      | Build the GUI application          | ON      |
| `BUILD_TOOLS`    | Build command-line tools           | ON      |
| `BUILD_WEBUI`    | Build the web UI                   | OFF     |
| `BUILD_ALL`      | Build all components               | OFF     |
| `BUILD_CLI_ONLY` | Build only core and tools (no GUI) | OFF     |
| `BUILD_GUI_ONLY` | Build only core and GUI (no tools) | OFF     |

## Running

### GUI Application
```bash
# Run the GUI application
./build/workbench/apps/gui/workbench_gui
```

### Command-Line Tools
```bash
# Show help for obj2usd converter
./build/workbench/apps/tools/converters/obj2usd/obj2usd --help
./build/workbench/apps/tools/converters/obj2usd/obj2usd input.obj output.usd

# Show help for mesh optimization tools
./build/workbench/apps/tools/optimizers/mesh/triangulate_meshes --help
./build/workbench/apps/tools/optimizers/mesh/remove_hidden_meshes --help
```

## Project Structure
```
├── workbench/              # Workbench domain
│   ├── libs/               # Workbench libraries
│   │   ├── core/           # Core library (shared functionality)
│   │   └── optimizer/      # USD optimization algorithms
│   └── apps/               # Workbench applications
│       ├── gui/            # Qt-based GUI application
│       ├── tools/          # Command-line tools
│       │   ├── converters/ # Format conversion tools
│       │   └── optimizers/ # Mesh optimization tools
│       └── webui/          # Web interface (optional)
├── conveyor/               # Conveyor domain (workflow orchestration)
│   ├── libs/               # Conveyor libraries
│   │   └── common/         # Shared utilities and types
│   └── apps/               # Conveyor applications
│       ├── orchestrator/   # Workflow orchestration service
│       └── worker/         # Task execution worker
├── shared/                 # Cross-domain shared code
├── resources/              # Test assets and sample files
└── build.sh               # Convenient build script
```
## Development

### Adding New Components

#### Adding Workbench Components
1. **Libraries**: Create directories under `workbench/libs/`
2. **Applications**: Create directories under `workbench/apps/`
3. Add a `CMakeLists.txt` file for your component
4. Update the main `CMakeLists.txt` to include your component
5. Add build options and tasks as needed

#### Cross-Domain Components
1. **Shared Libraries**: Use `shared/` for code used by both domains
2. Ensure clear dependency management between domains

### Standalone Tool Building
Individual tools can also be built standalone:

```bash
cd workbench/apps/tools/converters/obj2usd
mkdir build && cd build
cmake ..
make
```

## Notes
- The GUI application requires Qt5 to be installed and findable by CMake.
- Some tools require additional dependencies like FBX SDK or Assimp.
- Dependencies are automatically resolved when building as part of the main project.
