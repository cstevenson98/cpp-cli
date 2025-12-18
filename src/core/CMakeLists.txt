# Core CLI library
add_library(core_cli STATIC
    cli.cc
    fullscreen_terminal.cc
)

target_include_directories(core_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(core_cli PUBLIC cxx_std_17)

# Install the library and headers
install(TARGETS core_cli
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)

# Unit tests
add_subdirectory(unit_tests)

