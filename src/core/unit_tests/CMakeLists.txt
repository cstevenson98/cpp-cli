# Core CLI unit tests
add_executable(test_core_cli
    test_cli.cpp
)

target_link_libraries(test_core_cli PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_core_cli)

