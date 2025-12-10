# tachyon-manifold unit tests
add_executable(test_tachyon_manifold
    test_tachyon_manifold.cpp
    ../tachyon_manifold.cc
)

target_include_directories(test_tachyon_manifold PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/tachyon-manifold/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_tachyon_manifold PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_tachyon_manifold)

