# Mesh optimizer tools subdirectory

# Create executable for triangulate_meshes
add_executable(triangulate_meshes triangulate_meshes.cpp)

# Create executable for remove_hidden_meshes
add_executable(remove_hidden_meshes remove_hidden_meshes.cpp)

# Link against required libraries
target_link_libraries(triangulate_meshes
    PRIVATE
        workbench_core
        workbench_optimizer
        ${PXR_LIBRARIES}
)

target_link_libraries(remove_hidden_meshes
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

target_include_directories(remove_hidden_meshes
    PRIVATE
        ${PXR_INCLUDE_DIRS}
)

# Install the executables
install(TARGETS triangulate_meshes remove_hidden_meshes
    RUNTIME DESTINATION bin
)