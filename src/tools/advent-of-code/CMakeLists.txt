cmake_minimum_required(VERSION 3.22)

# Advent of Code CLI demo executable
add_executable(advent-of-code main.cpp)

target_link_libraries(advent-of-code PRIVATE core_cli)

target_include_directories(advent-of-code PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/core/include
)

# Install the executable
install(TARGETS advent-of-code DESTINATION bin)

