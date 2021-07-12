#include "shader.hh"

#include "tools.hh"
#include "vk_tools.hh"
#include "device.hh"

namespace vk
{
Handle<Shader> Device::create_shader(const std::string& path)
{
    auto shader_code = tools::read_file<std::vector<uint32_t>>(path + ".spv");

    VkShaderModuleCreateInfo shader_info{};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = shader_code.size() * sizeof(uint32_t);
    shader_info.pCode = shader_code.data();

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(vk_handle, &shader_info, nullptr, &vk_shader));

    return shaders.insert({.path = std::string(path), .vk_handle = vk_shader});
}

void Device::destroy_shader(const Handle<Shader>& handle)
{
    if (!handle.is_valid())
    {
        return;
    }

    Shader& shader = shaders.get(handle);
    vkDestroyShaderModule(vk_handle, shader.vk_handle, nullptr);
    shader.vk_handle = VK_NULL_HANDLE;
    shaders.remove(handle);
}
} // namespace vk
