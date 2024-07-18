cmake_minimum_required(VERSION 3.19)

# --- BUL ---
add_library(bul STATIC
    src/bul.cpp
    src/input.cpp
    src/hash.cpp
    src/log.cpp
    src/platform/file_win32.cpp
    src/platform/window_win32.cpp
    src/platform/time_win32.cpp
    src/allocators/linear_allocator.cpp
    src/allocators/scope_allocator.cpp

    third_party/xxhash/xxhash.c
 )

target_include_directories(bul
    PUBLIC include
    PRIVATE src third_party
)

target_link_libraries(bul PRIVATE default_interface)

set_target_properties(bul PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)