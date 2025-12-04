# printing unit tests
add_executable(test_printing
    test_printing.cpp
)

target_include_directories(test_printing PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/printing/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_printing PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_printing)



