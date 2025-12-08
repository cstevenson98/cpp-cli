# Fullscreen demo tool
add_executable(fullscreen_demo
    main.cpp
)

target_include_directories(fullscreen_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(fullscreen_demo PRIVATE
    core_cli
)

symlink_tool_to_root(fullscreen_demo)
