#include "imgui.h"

// #include <imgui/backends/imgui_impl_glfw.h>
// #include <imgui/backends/imgui_impl_vulkan.h>

#include "bul/window.h"

#include "context.h"
#include "device.h"
#include "surface.h"
#include "command.h"

namespace vk
{
void imgui_init(Context&, Device&, Surface&)
{
    /* ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImVec4* style_colors = ImGui::GetStyle().Colors;
    style_colors[ImGuiCol_Header] = ImVec4(0, 0, 0, 0);
    style_colors[ImGuiCol_HeaderHovered] = ImVec4(0, 0, 0, 0);
    style_colors[ImGuiCol_HeaderActive] = ImVec4(0, 0, 0, 0);

    ImGui_ImplGlfw_InitForVulkan(bul::window::handle(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = context.instance;
    init_info.PhysicalDevice = device.physical_device.vk_handle;
    init_info.Device = device.vk_handle;
    init_info.Queue = device.graphics_queue;
    init_info.DescriptorPool = device.descriptor_pool;
    init_info.MinImageCount = surface.images.size();
    init_info.ImageCount = surface.images.size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    const auto& renderpass = device.get_or_create_renderpass(surface.framebuffers[0], {vk::LoadOp::load()});
    ImGui_ImplVulkan_Init(&init_info, renderpass.vk_handle);

    auto& cmd = device.get_graphics_command();
    ImGui_ImplVulkan_CreateFontsTexture(cmd.vk_handle);
    device.submit_blocking(cmd);

    ImGui_ImplVulkan_DestroyFontUploadObjects(); */
}

void imgui_shutdown()
{
    /* ImGui_ImplVulkan_Shutdown(); */
}

void imgui_new_frame()
{
    /* ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame(); */
}

void imgui_render()
{
    /* ImGui::Render(); */
}

void imgui_render_draw_data(Command&)
{
    /* ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.vk_handle); */
}
} // namespace vk
