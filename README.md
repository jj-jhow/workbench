# Workbench

A modular C++ application suite using Pixar's OpenUSD (pxr) library for USD asset pipeline tools and GUI applications.

## Components

This project is organized into multiple components that can be built independently or together:

- **Core Library** (`apps/core/`): Shared functionality, converters, and USD utilities
- **GUI Application** (`apps/gui/`): Qt-based GUI for USD scene viewing and editing
- **Command-Line Tools** (`apps/tools/`): CLI utilities for USD conversion and processing
- **Web UI** (`apps/webui/`): Web-based interface (optional)

## Requirements
- CMake >= 3.20
- C++20 or newer
- OpenUSD (pxr) built and installed at `/home/jonathan/usd`
- Qt5 (for GUI components)
- FBX SDK (for FBX conversion tools)
- Assimp (for additional format support)

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
./build/apps/gui/workbench_gui
```

### Command-Line Tools
```bash
# obj2usd converter
./build/apps/tools/converters/obj2usd/obj2usd --help
./build/apps/tools/converters/obj2usd/obj2usd input.obj output.usd
```

## Project Structure
```
├── apps/
│   ├── core/           # Core library with shared functionality
│   ├── gui/            # Qt-based GUI application
│   ├── tools/          # Command-line tools
│   │   └── converters/
│   │       └── obj2usd/
│   └── webui/          # Web interface (optional)
├── resources/          # Test assets and sample files
├── build.sh           # Convenient build script
└── .vscode/
    └── tasks.json      # VS Code build tasks
```

## Development

### Adding New Components
1. Create a new directory under `apps/`
2. Add a `CMakeLists.txt` file
3. Update the main `CMakeLists.txt` to include your component
4. Add build options and VS Code tasks as needed

### Standalone Tool Building
Individual tools can also be built standalone:

```bash
cd apps/tools/converters/obj2usd
mkdir build && cd build
cmake ..
make
```

## Notes
- If you see include errors in your editor, ensure your includePath is set to `/home/jonathan/usd/include`.
- The GUI application requires Qt5 to be installed and findable by CMake.
- Some tools require additional dependencies like FBX SDK or Assimp.
- Dependencies are automatically resolved when building as part of the main project.
