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
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

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
target_link_libraries(${PROJECT_NAME} PRIVATE ${PXR_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PXR_INCLUDE_DIRS})

# --- oneTBB Setup ---
find_package(TBB REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)

# --- OpenSubdiv Setup ---
find_package(OpenSubdiv REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE ${TBB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${TBB_DIR})

# --- Qt Setup ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

# --- FBX SDK Setup ---
# set(FBX_SDK_ROOT "/Applications/Autodesk/FBX SDK/2020.3.7")
# set(FBX_SDK_INCLUDE_DIR "${FBX_SDK_ROOT}/include")
# set(FBX_SDK_LIB_DIR "${FBX_SDK_ROOT}/lib/clang/debug")

# target_include_directories(${PROJECT_NAME} PRIVATE ${FBX_SDK_INCLUDE_DIR})
# target_link_directories(${PROJECT_NAME} PRIVATE ${FBX_SDK_LIB_DIR})
# target_link_libraries(${PROJECT_NAME} PRIVATE fbxsdk)
