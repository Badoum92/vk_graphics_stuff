# --- Third party interface ---

add_library(third_party_interface INTERFACE)
target_compile_options(third_party_interface INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wno-everything>
)

# --- Volk ---

add_library(volk STATIC volk/volk.c)
target_link_libraries(volk PRIVATE third_party_interface)
target_include_directories(volk SYSTEM PUBLIC
    volk
    vulkan_headers/include
)
target_compile_definitions(volk PUBLIC
    $<$<BOOL:${WIN32}>:VK_USE_PLATFORM_WIN32_KHR>
    VK_NO_PROTOTYPES
)

# --- ImGui ---

file(GLOB IMGUI_SOURCES
    imgui/*.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE
    imgui/
    imgui/backends
    ${Vulkan_INCLUDE_DIRS}
)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

# --- ufbx ---

add_library(ufbx STATIC ufbx/ufbx.c)
target_link_libraries(ufbx PRIVATE third_party_interface)
target_include_directories(ufbx SYSTEM PRIVATE ufbx)
target_compile_definitions(ufbx PUBLIC UFBX_REAL_IS_FLOAT)