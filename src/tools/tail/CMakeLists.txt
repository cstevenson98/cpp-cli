# tail tool
add_executable(tail
    main.cpp
)

target_include_directories(tail PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tail PRIVATE
    core_cli
)

# Unit tests
add_subdirectory(unit_tests)

