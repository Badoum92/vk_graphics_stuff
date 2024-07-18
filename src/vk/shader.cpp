#include "shader.h"

#include "vk/context.h"

#include "bul/bul.h"
#include "bul/file.h"
#include "bul/allocators/scope_allocator.h"

namespace vk
{
bul::handle<shader> context::create_shader(const char* path)
{
    bul::scope_allocator scope_allocator = bul::scope_allocator::create_global();

    bul::file file = bul::file::open(path, bul::file::mode::read);
    defer
    {
        file.close();
    };
    uint32_t file_size = file.size();
    uint8_t* file_data = (uint8_t*)scope_allocator.alloc(file_size);
    file.read(file_data, file_size);

    VkShaderModuleCreateInfo shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = file_size;
    shader_info.pCode = (uint32_t*)file_data;

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &shader_info, nullptr, &vk_shader));

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
