cmake_minimum_required(VERSION 3.19)
project(vk_graphics_stuff LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:Clang>:-fansi-escape-codes>
)

add_library(default_interface INTERFACE)
target_compile_options(default_interface INTERFACE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -msse4.1>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zc:preprocessor>
)
target_compile_definitions(default_interface INTERFACE
    $<$<BOOL:${WIN32}>:NOMINMAX>
    $<$<BOOL:${WIN32}>:NOCOMM>
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN>
    $<$<BOOL:${WIN32}>:VC_EXTRALEAN>
    $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS>
)

# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

# --- Vulkan ---

find_package(Vulkan REQUIRED)

# --- Subdirectories ---

add_subdirectory(bul)
add_subdirectory(third_party)

# --- Executable ---

add_executable(main
    src/main.cpp

    src/util/implems.cpp

    src/engine/camera.cpp
    src/engine/image.cpp
    src/engine/texture.cpp
    # src/engine/renderer.cpp
    # src/engine/path_tracing_renderer.cpp
    src/engine/test_renderer.cpp
    src/engine/gltf.cpp
    src/engine/vox_loader.cpp

    src/vk/vk_tools.cpp
    src/vk/context.cpp
    src/vk/surface.cpp
    src/vk/image.cpp
    src/vk/buffer.cpp
    src/vk/shader.cpp
    src/vk/pipeline.cpp
    src/vk/descriptor.cpp
    src/vk/commands.cpp
)

target_include_directories(main
    PRIVATE src
    PRIVATE src/util
    PRIVATE src/engine

    PRIVATE .
)

target_include_directories(main
    SYSTEM PRIVATE ${Vulkan_INCLUDE_DIRS}
    SYSTEM PRIVATE third_party/imgui
    SYSTEM PRIVATE third_party
)

target_link_libraries(main
    default_interface
    imgui
    bul
    volk
    ufbx
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
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -g -I ${CMAKE_SOURCE_DIR}/shaders/include --target-env=vulkan1.3 -std=460 ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL} ${GLSL_INCLUDE_FILES}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(main shaders)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:main>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:main>/shaders"
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:main>/resources"
)
