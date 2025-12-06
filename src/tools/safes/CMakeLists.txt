# safes tool
add_executable(safes
    main.cpp
)

target_include_directories(safes PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(safes PRIVATE
    core_cli
)

symlink_tool_to_root(safes)

# Unit tests
add_subdirectory(unit_tests)

