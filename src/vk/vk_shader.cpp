#include "vk/vk_shader.h"

#include "vk/vk_context.h"

#include "core/core.h"
#include "core/file.h"
#include "core/memory/linear_allocator.h"

namespace vk
{
bul::handle<shader> context::create_shader(const char* path)
{
    linear_allocator* allocator = linear_allocator_get_global();

    file file = file_open_read(path);
    uint32_t size = (uint32_t)file_get_size(&file);
    uint8_t* data = (uint8_t*)linear_alloc(allocator, size);
    file_read(&file, data, size);
    file_close(&file);

    VkShaderModuleCreateInfo shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = size;
    shader_info.pCode = (uint32_t*)data;

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &shader_info, nullptr, &vk_shader));

    linear_free(allocator, data);
    return shaders.insert(shader{vk_shader, path});
}

void context::destroy_shader(bul::handle<shader> handle)
{
    shader& shader = shaders.get(handle);
    if (shader.vk_handle != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(device, shader.vk_handle, nullptr);
        shader.vk_handle = VK_NULL_HANDLE;
    }
    shaders.erase(handle);
}
} // namespace vk
