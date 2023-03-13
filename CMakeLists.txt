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
    src/main.cc

    src/util/implems.cc

    src/engine/window.cc
    src/engine/camera.cc
    src/engine/event.cc
    src/engine/input.cc
    src/engine/renderer.cc
    src/engine/path_tracing_renderer.cc
    src/engine/gltf.cc
    src/engine/vox_loader.cc

    src/engine/vulkan/vk_tools.cc
    src/engine/vulkan/context.cc
    src/engine/vulkan/device.cc
    src/engine/vulkan/surface.cc
    src/engine/vulkan/image.cc
    src/engine/vulkan/buffer.cc
    src/engine/vulkan/framebuffer.cc
    src/engine/vulkan/shader.cc
    src/engine/vulkan/graphics_pipeline.cc
    src/engine/vulkan/compute_pipeline.cc
    src/engine/vulkan/descriptor_set.cc
    src/engine/vulkan/command.cc
    src/engine/vulkan/frame_context.cc
    src/engine/vulkan/imgui.cc
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
    SYSTEM PRIVATE third_party/glfw/include
    SYSTEM PRIVATE third_party/glm
    SYSTEM PRIVATE third_party/imgui
    SYSTEM PRIVATE third_party/nlohmann/include
    SYSTEM PRIVATE third_party
)

target_link_libraries(main
    default_interface
    # ${Vulkan_LIBRARIES}
    glfw
    imgui
    bul
    volk
)

add_definitions(
    -DGLFW_INCLUDE_VULKAN
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

set_target_properties(main bul tests volk PROPERTIES
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
