# maxjoltage tool
add_executable(maxjoltage
    main.cpp
)

target_include_directories(maxjoltage PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(maxjoltage PRIVATE
    core_cli
)

# Unit tests
add_subdirectory(unit_tests)









