# movie-theatre tool
add_executable(movie-theatre
    main.cpp
)

target_include_directories(movie-theatre PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(movie-theatre PRIVATE
    core_cli
)

symlink_tool_to_root(movie-theatre)

# Unit tests
add_subdirectory(unit_tests)

