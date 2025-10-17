#!/bin/bash
# Sample configuration file for Workbench project dependencies
# Copy this file to ~/.workbench_config.sh and modify the paths as needed
# Then source it: source ~/.workbench_config.sh

# ============================================================
# REQUIRED: Pixar USD (OpenUSD) installation
# ============================================================
# Download and install from: https://github.com/PixarAnimationStudios/OpenUSD
export PXR_INSTALL_PATH="/home/jonathan/repos/OpenUSD/install"

# ============================================================
# REQUIRED: Assimp (Open Asset Import Library)
# ============================================================
# Option 1: Install via package manager (Ubuntu/Debian):
# sudo apt install libassimp-dev

# Option 2: Build from source and set custom path:
# export ASSIMP_ROOT="/path/to/your/assimp/installation"

# Option 3: Use system installation (default behavior - no export needed)
# If Assimp is installed system-wide, CMake will find it automatically

# ============================================================
# OPTIONAL: FBX SDK (Autodesk)
# ============================================================
# Download from: https://aps.autodesk.com/developer/overview/fbx-sdk
# Default fallback path if not set: /opt/fbx202037_fbxsdk_linux
export FBX_SDK_PATH="/opt/fbx202037_fbxsdk_linux"

# ============================================================
# USAGE INSTRUCTIONS
# ============================================================
# 1. Copy this file: cp config.sample.sh ~/.workbench_config.sh
# 2. Edit ~/.workbench_config.sh with your actual paths
# 3. Source it: echo "source ~/.workbench_config.sh" >> ~/.bashrc
# 4. Reload your shell: source ~/.bashrc
# 5. Build the project: ./build.sh all

echo "Workbench environment configured successfully!"
echo "USD Path: ${PXR_INSTALL_PATH}"
echo "Assimp Root: ${ASSIMP_ROOT:-'System default'}"
echo "FBX SDK Path: ${FBX_SDK_PATH}"