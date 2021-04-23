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
#     third_party/glad/include
# )
# target_link_libraries(imgui PRIVATE
#     ${OPENGL_LIBRARIES}
#     glfw
#     glad
# )
# set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
# add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
# ------------

add_executable(main
    src/main.cc
)

target_include_directories(main PRIVATE
    SYSTEM third_party/glfw/include
    SYSTEM third_party/glm
    SYSTEM third_party/imgui
    SYSTEM third_party
    SYSTEM ${Vulkan_INCLUDE_DIRS}
    src
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
