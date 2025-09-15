cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force GCC 11
set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")

project(workbench VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
	src/private/main.cpp
	src/private/StageManager.cpp
	src/private/converters/ConverterFactory.cpp
	src/private/converters/FbxToUsdConverter.cpp
	src/private/converters/UsdToFbxConverter.cpp
	src/private/gui/MainWindow.cpp
)

# Enable MOC for Qt (Qt's Meta-Object Compiler)
set(CMAKE_AUTOMOC ON)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/private
    ${CMAKE_SOURCE_DIR}/src/public
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)

# Set rpath for build and install so the binary in bin/ finds OpenUSD libs
set_target_properties(${PROJECT_NAME} PROPERTIES
	BUILD_RPATH "${OPENUSD_LIB_DIR}"
	INSTALL_RPATH "${OPENUSD_LIB_DIR}"
	INSTALL_RPATH_USE_LINK_PATH TRUE
)


set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# --- OpenUSD Setup ---
find_package(pxr REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE usd)
target_include_directories(${PROJECT_NAME} PRIVATE ${PXR_INCLUDE_DIRS})

# --- Qt Setup ---
# Fix Qt/TBB 'emit' conflict
add_definitions(-DQT_NO_EMIT)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui)
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

# --- FBX SDK Setup ---
# set(FBX_SDK_ROOT "/Applications/Autodesk/FBX SDK/2020.3.7")
# set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
# set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/clang/debug")

# target_include_directories(${PROJECT_NAME} PRIVATE ${FBX_SDK_INCLUDE_DIR})
# target_link_directories(${PROJECT_NAME} PRIVATE ${FBX_SDK_LIB_DIR})
# target_link_libraries(${PROJECT_NAME} PRIVATE fbxsdk)
