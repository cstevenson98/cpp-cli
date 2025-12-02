# cp_file unit tests
add_executable(test_cp_file
    test_cp_file.cpp
)

target_include_directories(test_cp_file PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/cp_file/include
)

target_link_libraries(test_cp_file PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_cp_file)

