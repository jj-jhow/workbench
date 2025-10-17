if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0144 NEW)  # Use UPPER_CASE_ROOT variables
endif(COMMAND cmake_policy)

# Define a target for our core library.
# We list its source files here. It's better to list them explicitly
# than to use GLOB, as it ensures new files are intentionally added.
add_library(workbench_core STATIC
	src/private/converters/ConverterFactory.cpp
	src/private/converters/FbxToUsdConverter.cpp
	src/private/converters/ObjToUsdConverter.cpp
	src/private/converters/UsdToFbxConverter.cpp
	src/private/converters/UpAxis.cpp
	src/private/converters/LinearUnit.cpp
	src/private/importers/FbxImporter.cpp
	src/private/StageManager.cpp

)


# --- Dependencies ---

# Define the path to dependency installations
# Allow users to specify FBX SDK location via environment variable or CMake variable
if(DEFINED ENV{FBX_SDK_PATH})
    set(FBX_SDK_PATH "$ENV{FBX_SDK_PATH}")
elseif(NOT FBX_SDK_PATH)
    set(FBX_SDK_PATH "/opt/fbx202037_fbxsdk_linux")  # Default fallback
endif()

# Allow users to specify Assimp location via environment variable or CMake variable
if(DEFINED ENV{ASSIMP_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{ASSIMP_ROOT}")
elseif(ASSIMP_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${ASSIMP_ROOT}")
endif()

# Link this library to its external dependencies.
find_package(pxr REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Find Assimp - try multiple methods for maximum compatibility
find_package(assimp CONFIG QUIET)
if(NOT assimp_FOUND)
    # Fallback to PkgConfig
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASSIMP QUIET assimp)
    endif()
    
    # Last resort: manual search
    if(NOT ASSIMP_FOUND)
        find_path(ASSIMP_INCLUDE_DIR assimp/Importer.hpp
            PATHS
                /usr/include
                /usr/local/include
                /opt/local/include
        )
        find_library(ASSIMP_LIBRARY
            NAMES assimp libassimp
            PATHS
                /usr/lib
                /usr/local/lib
                /opt/local/lib
        )
        if(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
            set(ASSIMP_FOUND TRUE)
            add_library(assimp::assimp UNKNOWN IMPORTED)
            set_target_properties(assimp::assimp PROPERTIES
                IMPORTED_LOCATION "${ASSIMP_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIR}"
            )
        endif()
    endif()
endif()

if(NOT assimp_FOUND AND NOT ASSIMP_FOUND)
    message(FATAL_ERROR "Assimp not found. Please install assimp or set ASSIMP_ROOT environment variable.")
endif()

if(pxr_FOUND)
    message(STATUS "Found Pixar USD")
    message(STATUS "pxr_DIR: ${pxr_DIR}")
    message(STATUS "PXR_INCLUDE_DIRS: ${PXR_INCLUDE_DIRS}")
    message(STATUS "PXR_LIBRARIES: ${PXR_LIBRARIES}")
else()
    message(FATAL_ERROR "Could not find Pixar USD. Please ensure it is installed and the PXR_INSTALL_PATH environment variable is set correctly.")
endif()

# --- Include Directories ---
target_include_directories( workbench_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/private
	PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/public
        ${PXR_INCLUDE_DIRS}
        ${FBX_SDK_PATH}/include
)

# Include the fbxsdk.cmake script to set up the FBX SDK targets
include(${FBX_SDK_PATH}/samples/fbxsdk.cmake)

target_link_libraries(workbench_core
    PUBLIC
        usd
        tf
        usdGeom
        fbxsdk
        usdImagingGL
        assimp::assimp
)

# --- Installation Rules ---
# This section defines how to install this library.
install(TARGETS workbench_core
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/public/
    DESTINATION include
)

# get_cmake_property(_vars VARIABLES)
# foreach(_var IN LISTS _vars)
#     if(_var MATCHES "^PXR")
#         message(STATUS "${_var}=${${_var}}")
#     endif()
# endforeach()
