# safes unit tests
add_executable(test_safes
    test_safes.cpp
)

target_include_directories(test_safes PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/safes/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_safes PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_safes)

