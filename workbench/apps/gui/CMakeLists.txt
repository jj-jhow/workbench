set(CMAKE_AUTOMOC ON) # Needed for Qt signals/slots
set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/include")

# --- Dependencies ---
# You will also need to find Qt packages here
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Define the executable target and its source files.
add_executable(workbench_gui
    src/main.cpp
    src/MainWindow.cpp
    src/SceneViewWidget.cpp
    src/SceneTreeWidget.cpp
    src/PrimPropertiesWidget.cpp
    src/render/HydraRenderer.cpp
    
    # Header files needed for Qt MOC
    include/MainWindow.h
    include/SceneViewWidget.h
    include/SceneTreeWidget.h
    include/PrimPropertiesWidget.h
    include/render/HydraRenderer.h
    # Add other source files for the GUI here
)

# --- Include Directories ---
target_include_directories(workbench_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5OpenGL_INCLUDE_DIRS}
)

# --- Link Libraries ---
target_link_libraries(workbench_gui
    PRIVATE
        # Link to our internal library by its target name
        workbench_core

        # Link to external dependencies like Qt
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::OpenGL
)


add_definitions(-DQT_NO_EMIT)

# --- Installation Rules ---
install(TARGETS workbench_gui
    RUNTIME DESTINATION bin
)
