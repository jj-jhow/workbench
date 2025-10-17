set(CMAKE_AUTOMOC ON) # Needed for Qt signals/slots
set(CMAKE_AUTOMOC_MOC_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}/src")

# --- Dependencies ---
# You will also need to find Qt packages here
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Define the executable target and its source files.
add_executable(workbench_gui
    # Main entry point
    src/main.cpp
    
    # Core components
    src/core/MainWindow.cpp
    
    # UI managers
    src/ui/MenuManager.cpp
    src/ui/ToolbarManager.cpp
    src/ui/StatusManager.cpp
    src/ui/LogManager.cpp
    
    # Action handlers
    src/actions/FileActions.cpp
    
    # Widgets
    src/widgets/SceneViewWidget.cpp
    src/widgets/SceneTreeWidget.cpp
    src/widgets/PrimPropertiesWidget.cpp
    
    # Rendering
    src/render/HydraRenderer.cpp
    
    # Header files needed for Qt MOC
    src/core/MainWindow.h
    src/ui/MenuManager.h
    src/ui/ToolbarManager.h
    src/ui/StatusManager.h
    src/ui/LogManager.h
    src/actions/FileActions.h
    src/widgets/SceneViewWidget.h
    src/widgets/SceneTreeWidget.h
    src/widgets/PrimPropertiesWidget.h
    src/render/HydraRenderer.h
)

# --- Include Directories ---
target_include_directories(workbench_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5OpenGL_INCLUDE_DIRS}
)

# --- Link Libraries ---
target_link_libraries(workbench_gui
    PUBLIC
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
