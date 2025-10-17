# Optimizer tools subdirectory

# Options for individual optimizer tools
option(BUILD_MESH_OPTIMIZERS "Build mesh optimization tools" ON)

if(BUILD_MESH_OPTIMIZERS)
    message(STATUS "Adding mesh optimizer tools")
    add_subdirectory(mesh)
endif()

# Add more optimizer tool categories here as they are developed
# Example:
# option(BUILD_TEXTURE_OPTIMIZERS "Build texture optimization tools" OFF)
# if(BUILD_TEXTURE_OPTIMIZERS)
#     add_subdirectory(texture)
# endif()