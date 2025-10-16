if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Define a target for our optimizer library.
add_library(workbench_optimizer STATIC
    src/MeshTriangulator.cpp
)

# --- Dependencies ---
# The optimizer depends on the core library and USD
find_package(pxr REQUIRED CONFIG)

if(pxr_FOUND)
    message(STATUS "Found Pixar USD for optimizer")
else()
    message(FATAL_ERROR "Could not find Pixar USD. Please ensure it is installed and the PXR_INSTALL_PATH environment variable is set correctly.")
endif()

# --- Include Directories ---
target_include_directories(workbench_optimizer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PXR_INCLUDE_DIRS}
)

# --- Link Libraries ---
target_link_libraries(workbench_optimizer
    PUBLIC
        usd
        usdGeom
        tf
        vt
        sdf
        workbench_core
)

# --- Installation Rules ---
install(TARGETS workbench_optimizer
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
)