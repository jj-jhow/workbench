if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Define a target for our core library.
# We list its source files here. It's better to list them explicitly
# than to use GLOB, as it ensures new files are intentionally added.
add_library(workbench_core STATIC
	src/private/converters/ConverterFactory.cpp
	src/private/converters/FbxToUsdConverter.cpp
	src/private/converters/UsdToFbxConverter.cpp
	src/private/importers/FbxImporter.cpp
	src/private/StageManager.cpp

)


# --- Dependencies ---
# Link this library to its external dependencies.
find_package(pxr REQUIRED CONFIG)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Define the path to your FBX SDK installation
set(FBX_SDK_PATH "/opt/fbx202037_fbxsdk_linux")

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
        fbxsdk
        usdImagingGL
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
