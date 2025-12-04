# printing tool
add_executable(printing
    main.cpp
)

target_include_directories(printing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(printing PRIVATE
    core_cli
)

# Unit tests
add_subdirectory(unit_tests)



