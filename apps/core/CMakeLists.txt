# Define a target for our core library.
# We list its source files here. It's better to list them explicitly
# than to use GLOB, as it ensures new files are intentionally added.
add_library(workbench_core STATIC
	src/private/converters/ConverterFactory.cpp
	src/private/converters/FbxToUsdConverter.cpp
	src/private/converters/UsdToFbxConverter.cpp
	src/private/StageManager.cpp

)

# --- Include Directories ---
# This command controls which directories are added to the include path.
# Using PUBLIC, PRIVATE, and INTERFACE is key to a clean monorepo.
target_include_directories(workbench_core
    # PRIVATE: For internal implementation. Only this library needs this path.
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/private

    # PUBLIC: For the public API. Anyone linking to workbench_core will also
    # get this include path, allowing them to #include "converters/ConverterFactory.h".
    PUBLIC
        # $<BUILD_INTERFACE:...> is for the build tree.
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
        # $<INSTALL_INTERFACE:...> is for after installation.
        $<INSTALL_INTERFACE:include>
)

# --- Dependencies ---
# Link this library to its external dependencies.
find_package(pxr REQUIRED CONFIG)
target_link_libraries(workbench_core PUBLIC usd)

# --- Installation Rules ---
# This section defines how to install this library.
install(TARGETS workbench_core
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/public/
    DESTINATION include
)