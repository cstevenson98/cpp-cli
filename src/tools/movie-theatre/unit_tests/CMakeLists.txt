# movie-theatre unit tests
add_executable(test_movie_theatre
    test_movie_theatre.cpp
)

target_include_directories(test_movie_theatre PRIVATE
    ${CMAKE_SOURCE_DIR}/src/tools/movie-theatre/include
    ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_movie_theatre PRIVATE
    core_cli
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_movie_theatre)

