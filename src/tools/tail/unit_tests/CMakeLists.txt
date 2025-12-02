# tail unit tests
add_executable(test_tail
    test_tail.cpp
)

target_include_directories(test_tail PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/tail/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_tail PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_tail)

