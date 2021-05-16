#include "vk_imgui.hh"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include "vk_context/vk_context.hh"
#include "window/window.hh"

void VkImgui::init()
{
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForVulkan(Window::handle(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VkContext::instance;
    init_info.PhysicalDevice = VkContext::physical_device;
    init_info.Device = VkContext::device;
    init_info.Queue = VkContext::device.graphics_queue();
    init_info.DescriptorPool = VkContext::descriptor_pool;
    init_info.MinImageCount = VkContext::swapchain.size();
    init_info.ImageCount = VkContext::swapchain.size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, VkContext::renderpass);

    vk_execute_once([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void VkImgui::shutdown()
{
    ImGui_ImplVulkan_Shutdown();
}

void VkImgui::new_frame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VkImgui::render()
{
    ImGui::Render();
}

void VkImgui::render_draw_data(const CommandBuffer& cmd)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void VkImgui::window(const std::string& name, const std::function<void()>& f)
{
    if (!ImGui::Begin(name.c_str()))
    {
        ImGui::End();
        return;
    }
    f();
    ImGui::End();
}
