cmake_minimum_required(VERSION 3.4)

project(bul)

# --- BUL ---
add_library(bul STATIC
    src/bul.cpp
    src/input.cpp
    src/math/vector.cpp
    src/math/matrix.cpp
    src/platform/util_win32.cpp
    src/platform/window_win32.cpp
    src/platform/timer_win32.cpp
)

target_include_directories(bul
    PUBLIC include
    PRIVATE src
)

target_link_libraries(bul PRIVATE default_interface)

# --- TESTS ---
if(BUILD_TESTS)
add_executable(tests
    tests/main.cpp
    tests/static_vec.cpp
    tests/vector.cpp
    tests/matrix.cpp
    tests/pool.cpp
)

target_link_libraries(tests
    bul
)
endif()
