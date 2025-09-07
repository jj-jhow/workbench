# Enable Qt meta-object compiler (moc) and UI compiler (uic)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# Find Qt5 Widgets
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt5")
find_package(Qt5 COMPONENTS Widgets REQUIRED)
# Find Python3 and add its include directory
find_package(Python3 COMPONENTS Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})
cmake_minimum_required(VERSION 3.15)
project(usd_console_tool)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set USD root directory
set(USD_ROOT "/home/jonathan/usd")


# Add USD's CMake config path for bundled dependencies like TBB
list(APPEND CMAKE_PREFIX_PATH "${USD_ROOT}" "${USD_ROOT}/lib/cmake" "${USD_ROOT}/lib/cmake/TBB")
find_package(pxr REQUIRED CONFIG PATHS "${USD_ROOT}")


add_definitions(${pxr_DEFINITIONS})


set(SOURCES
	src/main.cpp
	src/mainwindow.cpp
	src/primtreeview.cpp
	src/propertytableview.cpp
	src/usdviewport.cpp
)
set(HEADERS
	src/mainwindow.h
	src/primtreeview.h
	src/propertytableview.h
	src/usdviewport.h
)

add_executable(usd_console_tool ${SOURCES} ${HEADERS})
target_include_directories(usd_console_tool PRIVATE
	${pxr_INCLUDE_DIRS}
	/home/jonathan/usd/include
	${Python3_INCLUDE_DIRS}
)
link_directories(${pxr_LIBRARY_DIRS})

# Ensure linker can find all USD libraries
link_directories(${pxr_LIBRARY_DIRS})
target_link_libraries(usd_console_tool
	Qt5::Widgets
	${pxr_LIBRARIES}
	usd tf arch plug kind js work tbb
	Python3::Python
)
