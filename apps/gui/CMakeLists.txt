set(CMAKE_AUTOMOC ON) # Needed for Qt signals/slots

# --- Dependencies ---
# You will also need to find Qt packages here
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Define the executable target and its source files.
add_executable(workbench_gui
    src/main.cpp
    src/MainWindow.cpp
    # Add other source files for the GUI here
)

target_include_directories(workbench_gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
)

# This is where the magic happens. We link our application to the library
# target we defined in the other directory. CMake automatically handles
# include paths and library locations because of the PUBLIC/INTERFACE
# properties we set on the workbench_core target.
target_link_libraries(workbench_gui
    PRIVATE
        # Link to our internal library by its target name
        workbench_core

        # Link to external dependencies like Qt
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
)


add_definitions(-DQT_NO_EMIT)

# --- Installation Rules ---
install(TARGETS workbench_gui
    RUNTIME DESTINATION bin
)
