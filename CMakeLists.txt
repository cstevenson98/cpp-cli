cmake_minimum_required(VERSION 3.21)
project(cpp-cli VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTest from Conan
find_package(GTest REQUIRED)

# Enable testing
enable_testing()

# Add core library
add_subdirectory(src/core)

# Auto-discover and add all tools
file(GLOB TOOL_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/src/tools ${CMAKE_SOURCE_DIR}/src/tools/*)
foreach(TOOL_DIR ${TOOL_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tools/${TOOL_DIR})
        add_subdirectory(src/tools/${TOOL_DIR})
    endif()
endforeach()

# Create symlink for compile_commands.json in project root for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

