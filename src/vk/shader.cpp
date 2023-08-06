#include "shader.h"

#include <string_view>

#include "bul/file.h"

#include "vk_tools.h"
#include "context.h"

namespace vk
{
bul::Handle<Shader> Context::create_shader(const std::string_view path)
{
    std::vector<uint8_t> shader_code;
    std::string spirv_path{path.data(), path.size()};
    spirv_path += ".spv";
    bul::read_file(spirv_path.c_str(), shader_code);

    VkShaderModuleCreateInfo shader_info{};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = shader_code.size();
    shader_info.pCode = (uint32_t*)shader_code.data();

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &shader_info, nullptr, &vk_shader));

    return shaders.insert({.path = std::move(spirv_path), .vk_handle = vk_shader});
}

void Context::destroy_shader(bul::Handle<Shader> handle)
{
    Shader& shader = shaders.get(handle);
    vkDestroyShaderModule(device, shader.vk_handle, nullptr);
    shader.vk_handle = VK_NULL_HANDLE;
}
} // namespace vk
