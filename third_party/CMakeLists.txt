# --- Third party interface ---

add_library(third_party_interface INTERFACE)
target_compile_options(third_party_interface INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wno-everything>
)

# --- Volk ---

add_library(volk volk/volk.c)
target_link_libraries(volk PRIVATE third_party_interface)
target_include_directories(volk SYSTEM PUBLIC
    volk
    vulkan_headers/include
)
target_compile_definitions(volk PUBLIC
    $<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
    VK_NO_PROTOTYPES
)

# --- GLFW ---

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(glfw)

# --- IMGUI ---

file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_vulkan.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE
    imgui/
    imgui/backends
    glfw/include
    ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(imgui PRIVATE
    glfw
)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
