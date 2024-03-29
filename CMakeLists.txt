cmake_minimum_required(VERSION 3.19)
project(vk_graphics_stuff LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:Clang>:-fansi-escape-codes>
)

add_library(default_interface INTERFACE)
target_compile_options(default_interface INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-pedantic>
)

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
    src/engine/renderer.cpp
    src/engine/path_tracing_renderer.cpp
    src/engine/gltf.cpp
    src/engine/vox_loader.cpp

    src/engine/vulkan/vk_tools.cpp
    src/engine/vulkan/context.cpp
    src/engine/vulkan/device.cpp
    src/engine/vulkan/surface.cpp
    src/engine/vulkan/image.cpp
    src/engine/vulkan/buffer.cpp
    src/engine/vulkan/framebuffer.cpp
    src/engine/vulkan/shader.cpp
    src/engine/vulkan/graphics_pipeline.cpp
    src/engine/vulkan/compute_pipeline.cpp
    src/engine/vulkan/descriptor_set.cpp
    src/engine/vulkan/command.cpp
    src/engine/vulkan/frame_context.cpp
    src/engine/vulkan/imgui.cpp
)

target_include_directories(main
    PRIVATE src
    PRIVATE src/util
    PRIVATE src/engine
    PRIVATE src/engine/vulkan

    PRIVATE .
)

target_include_directories(main
    SYSTEM PRIVATE ${Vulkan_INCLUDE_DIRS}
    SYSTEM PRIVATE third_party/glm
    SYSTEM PRIVATE third_party/imgui
    SYSTEM PRIVATE third_party/nlohmann/include
    SYSTEM PRIVATE third_party
)

target_link_libraries(main
    default_interface
    imgui
    bul
    volk
)

add_definitions(
    -DGLM_FORCE_RADIANS
    -DGLM_FORCE_DEPTH_ZERO_TO_ONE
    -DGLM_FORCE_RIGHT_HANDED
)

target_compile_definitions(main PRIVATE
    $<$<BOOL:${WIN32}>:NOMINMAX>
    $<$<BOOL:${WIN32}>:NOCOMM>
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN>
    $<$<BOOL:${WIN32}>:VC_EXTRALEAN>
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
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} -I ${CMAKE_SOURCE_DIR}/shaders/include --target-env=vulkan1.2 -std=460 ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL} ${GLSL_INCLUDE_FILES}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(main shaders)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:main>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_BINARY_DIR}/shaders" "$<TARGET_FILE_DIR:main>/shaders"
)
