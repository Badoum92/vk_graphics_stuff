#include "descriptor_pool.hh"

#include <array>

#include "vk_context/vk_context.hh"

const uint32_t DESCRIPTOR_COUNT = 128;

void DescriptorPool::create()
{
    std::array pool_sizes{
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = DESCRIPTOR_COUNT},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = DESCRIPTOR_COUNT},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = DESCRIPTOR_COUNT},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = DESCRIPTOR_COUNT},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = DESCRIPTOR_COUNT},
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = DESCRIPTOR_COUNT;

    VK_CHECK(vkCreateDescriptorPool(VkContext::device, &pool_info, nullptr, &handle_));
}

void DescriptorPool::destroy()
{
    vkDestroyDescriptorPool(VkContext::device, handle_, nullptr);
}
