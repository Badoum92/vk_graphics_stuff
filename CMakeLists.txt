cmake_minimum_required(VERSION 3.14)
enable_language(C CXX)
project(vk_graphics_stuff CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic -g3")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif ()

if (${CMAKE_GENERATOR} STREQUAL "Ninja")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -fansi-escape-codes")
endif ()

if (WIN32)
    set(CMAKE_C_SIMULATE_ID "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-microsoft -Wno-language-extension-token -Wno-gnu-anonymous-struct -Wno-nested-anon-types")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

# --- VULKAN ---
find_package(Vulkan REQUIRED)
# --------------

# --- GLFW ---
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(third_party/glfw)
# ------------

# --- IMGUI ---
# file(GLOB IMGUI_SOURCES
#     third_party/imgui/*.cpp
#     third_party/imgui/backends/imgui_impl_glfw.cpp
#     third_party/imgui/backends/imgui_impl_opengl3.cpp
# )
# add_library(imgui STATIC ${IMGUI_SOURCES})
# target_include_directories(imgui PRIVATE
#     third_party/imgui/
#     third_party/imgui/backends
#     third_party/glfw/include
# )
# target_link_libraries(imgui PRIVATE
#     ${OPENGL_LIBRARIES}
#     glfw
# )
# set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
# add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
# ------------

add_executable(main
    src/main.cc

    src/implems.cc

    src/api/window/window.cc
    src/api/vk_context/vk_context.cc
    src/api/instance/instance.cc
    src/api/surface/surface.cc
    src/api/physical_device/physical_device.cc
    src/api/device/device.cc
    src/api/swapchain/swapchain.cc
    src/api/pipeline/pipeline.cc
    src/api/shader/shader.cc
    src/api/render_pass/render_pass.cc
    src/api/framebuffer/framebuffer.cc
    src/api/command_pool/command_pool.cc
    src/api/command_buffers/command_buffers.cc
    src/api/buffer/buffer.cc
)

target_include_directories(main
    PRIVATE src
    PRIVATE src/api
)

target_include_directories(main
    SYSTEM PRIVATE ${Vulkan_INCLUDE_DIRS}
    SYSTEM PRIVATE third_party/glfw/include
    SYSTEM PRIVATE third_party/glm
    SYSTEM PRIVATE third_party/imgui
    SYSTEM PRIVATE third_party
)

target_link_libraries(main
    ${Vulkan_LIBRARIES}
    glfw
#    imgui
)

add_definitions(
    -DGLFW_INCLUDE_NONE
    -DGLFW_INCLUDE_VULKAN
    -DGLM_FORCE_RADIANS
    -DGLM_FORCE_DEPTH_ZERO_TO_ONE
)


# --- SHADERS ---
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.vert"
    "shaders/*.frag"
    "shaders/*.comp"
    "shaders/*.geom"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(main shaders)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:main>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:main>/shaders"
)
# ---------------