# Mesh optimizer tools subdirectory

# Create executable for triangulate_meshes
add_executable(triangulate_meshes triangulate_meshes.cpp)

# Link against required libraries
target_link_libraries(triangulate_meshes
    PRIVATE
        workbench_core
        workbench_optimizer
        ${PXR_LIBRARIES}
)

# Include directories
target_include_directories(triangulate_meshes
    PRIVATE
        ${PXR_INCLUDE_DIRS}
)

# Install the executable
install(TARGETS triangulate_meshes
    RUNTIME DESTINATION bin
)