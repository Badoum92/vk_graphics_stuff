cmake_minimum_required(VERSION 3.19)
project(vk_graphics_stuff LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:Clang>:-fansi-escape-codes>
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
)

add_library(default_interface INTERFACE)
target_compile_options(default_interface INTERFACE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -msse4.1>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zc:preprocessor>
)
target_compile_definitions(default_interface INTERFACE
    $<$<BOOL:${WIN32}>:NOMINMAX>
    $<$<BOOL:${WIN32}>:NOCOMM>
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN>
    $<$<BOOL:${WIN32}>:VC_EXTRALEAN>
    $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>
)

# --- Vulkan ---

find_package(Vulkan REQUIRED)

# --- Subdirectories ---

add_subdirectory(third_party)

# --- Executable ---

add_executable(main
    src/main.cpp
    src/pch.h
    src/globals.cpp
    src/globals.h

    src/util/implems.cpp

    src/engine/camera.cpp
    src/engine/camera.h
    src/engine/image.cpp
    src/engine/image.h
    src/engine/texture.cpp
    src/engine/texture.h
    src/engine/gltf.cpp
    src/engine/gltf.h
    src/engine/vox.cpp
    src/engine/vox.h
    src/engine/imgui.cpp
    src/engine/imgui.h

    # src/engine/renderer.cpp
    # src/engine/renderer.h
    # src/engine/path_tracing_renderer.cpp
    # src/engine/path_tracing_renderer.h
    src/engine/test_renderer.cpp
    src/engine/test_renderer.h
    src/engine/test_compute.cpp
    src/engine/test_compute.h

    src/vk/vk_tools.cpp
    src/vk/vk_tools.h
    src/vk/vk_context.cpp
    src/vk/vk_context.h
    src/vk/vk_surface.cpp
    src/vk/vk_surface.h
    src/vk/vk_image.cpp
    src/vk/vk_image.h
    src/vk/vk_buffer.cpp
    src/vk/vk_buffer.h
    src/vk/vk_shader.cpp
    src/vk/vk_shader.h
    src/vk/vk_pipeline.cpp
    src/vk/vk_pipeline.h
    src/vk/vk_descriptor.cpp
    src/vk/vk_descriptor.h
    src/vk/vk_commands.cpp
    src/vk/vk_commands.h

    src/core/core.cpp
    src/core/core.h
    src/core/input.cpp
    src/core/input.h
    src/core/log.cpp
    src/core/log.h
    src/core/thread.h
    src/core/rwlock.h
    src/core/event.h
    src/core/file.h
    src/core/window.h
    src/core/time.h
    src/core/math/math.h
    src/core/math/matrix.h
    src/core/math/vector.h
    src/core/memory/linear_allocator.cpp
    src/core/memory/linear_allocator.h
    src/core/containers/pool.h
    src/core/containers/span.h
    src/core/containers/vector.h

    src/core/platform/thread_win32.cpp
    src/core/platform/rwlock_win32.cpp
    src/core/platform/file_win32.cpp
    src/core/platform/window_win32.cpp
    src/core/platform/time_win32.cpp
)

target_include_directories(main
    PRIVATE src
    PRIVATE src/tracy
    PRIVATE src/util
    PRIVATE src/engine

    PRIVATE .
)

target_include_directories(main
    SYSTEM PRIVATE ${Vulkan_INCLUDE_DIRS}
    SYSTEM PRIVATE third_party
)

target_precompile_headers(main PRIVATE src/pch.h)

target_link_libraries(main
    default_interface
    imgui
    volk
    ufbx
    tracy
)

set_target_properties(main PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)

# --- Shaders ---

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.vert"
    "shaders/*.frag"
    "shaders/*.comp"
    "shaders/*.geom"
)

file(GLOB_RECURSE GLSL_INCLUDE_FILES "shaders/include/*")

foreach(GLSL ${GLSL_SOURCE_FILES})
    message("FILE: ${GLSL}")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_SOURCE_DIR}/shaders/spv/${FILE_NAME}")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -g -I ${CMAKE_SOURCE_DIR}/shaders/include --target-env=vulkan1.4 -std=460 ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL} ${GLSL_INCLUDE_FILES}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})
target_sources(shaders PRIVATE ${GLSL_INCLUDE_FILES} ${GLSL_SOURCE_FILES})
add_dependencies(main shaders)

add_custom_command(TARGET main POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:main>/shaders"
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:main>/shaders"
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:main>/resources"
)
