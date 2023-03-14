cmake_minimum_required(VERSION 3.19)

# --- BUL ---
add_library(bul STATIC
    src/bul.cpp
    src/file.cpp
    src/input.cpp
    src/platform/util_win32.cpp
    src/platform/window_win32.cpp
    src/platform/time_win32.cpp
)

target_include_directories(bul
    PUBLIC include
    PRIVATE src
)

target_link_libraries(bul PRIVATE default_interface)

target_compile_definitions(bul PRIVATE
    $<$<BOOL:${WIN32}>:NOMINMAX>
    $<$<BOOL:${WIN32}>:NOCOMM>
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN>
    $<$<BOOL:${WIN32}>:VC_EXTRALEAN>
)

set_target_properties(bul PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)

# --- TESTS ---
add_executable(tests
    tests/main.cpp
    tests/static_vector.cpp
    tests/static_queue.cpp
    tests/vector.cpp
    tests/matrix.cpp
    tests/pool.cpp
    tests/map.cpp
)

target_link_libraries(tests
    bul
)

set_target_properties(tests PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)
