# cp_file tool
add_executable(cp_file
    main.cpp
)

target_include_directories(cp_file PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cp_file PRIVATE
    core_cli
)

symlink_tool_to_root(cp_file)

# Unit tests
add_subdirectory(unit_tests)

