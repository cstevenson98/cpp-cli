# tachyon-manifold tool
add_executable(tachyon-manifold
    main.cpp
    tachyon_manifold.cc
)

target_include_directories(tachyon-manifold PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tachyon-manifold PRIVATE
    core_cli
)

symlink_tool_to_root(tachyon-manifold)
