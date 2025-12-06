# cephalopods unit tests
add_executable(test_cephalopods
    test_cephalopods.cpp
)

target_include_directories(test_cephalopods PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/cephalopods/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_cephalopods PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_cephalopods)

