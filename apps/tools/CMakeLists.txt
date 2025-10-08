# Tools subdirectory
# This file includes all the command-line tools

# Options for individual tools
option(BUILD_OBJ2USD "Build the obj2usd converter tool" ON)

if(BUILD_OBJ2USD)
    message(STATUS "Adding obj2usd converter tool")
    add_subdirectory(converters/obj2usd)
endif()

# Add more tools here as they are developed
# Example:
# option(BUILD_USD2OBJ "Build the usd2obj converter tool" OFF)
# if(BUILD_USD2OBJ)
#     add_subdirectory(converters/usd2obj)
# endif()