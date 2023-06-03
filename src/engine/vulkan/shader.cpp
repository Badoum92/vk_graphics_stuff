#include "shader.h"

#include "bul/file.h"

#include "vk_tools.h"
#include "device.h"

namespace vk
{
bul::Handle<Shader> Device::create_shader(const std::string& path)
{
    std::vector<uint8_t> shader_code;
    bul::read_file((path + ".spv").c_str(), shader_code);

    VkShaderModuleCreateInfo shader_info{};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = shader_code.size();
    shader_info.pCode = (uint32_t*)shader_code.data();

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(vk_handle, &shader_info, nullptr, &vk_shader));

    return shaders.insert({.path = std::string(path), .vk_handle = vk_shader});
}

void Device::destroy_shader(Shader& shader)
{
    vkDestroyShaderModule(vk_handle, shader.vk_handle, nullptr);
    shader.vk_handle = VK_NULL_HANDLE;
}
} // namespace vk
