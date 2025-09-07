# USD Console Tool

A minimal C++ console application using Pixar's OpenUSD (pxr) library for USD asset pipeline tools.

## Requirements
- CMake >= 3.15
- C++14 or newer
- OpenUSD (pxr) built and installed at `/home/jonathan/usd`

## Building
```bash
mkdir -p build
cd build
cmake ..
make
```

## Running
```bash
./usd_console_tool
```

## Notes
- If you see include errors in your editor, ensure your includePath is set to `/home/jonathan/usd/include`.
- The application creates an in-memory USD stage as a basic test of the pxr integration.
