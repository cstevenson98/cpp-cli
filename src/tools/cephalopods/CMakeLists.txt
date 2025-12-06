# cephalopods tool
add_executable(cephalopods
    main.cpp
)

target_include_directories(cephalopods PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cephalopods PRIVATE
    core_cli
)

symlink_tool_to_root(cephalopods)

# Unit tests
add_subdirectory(unit_tests)

