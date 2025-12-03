# maxjoltage unit tests
add_executable(test_maxjoltage
    test_maxjoltage.cpp
)

target_include_directories(test_maxjoltage PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/maxjoltage/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_maxjoltage PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_maxjoltage)





