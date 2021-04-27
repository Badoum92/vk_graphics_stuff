#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class SwapChain
{
public:
    void create();
    void destroy();

    const VkFormat& format() const;
    const VkExtent2D& extent() const;
    const std::vector<VkImageView>& image_views() const;
    size_t size() const;

    inline VkSwapchainKHR handle() const
    {
        return handle_;
    }

    inline operator VkSwapchainKHR() const
    {
        return handle_;
    }

private:
    VkSwapchainKHR handle_;
    VkFormat format_;
    VkExtent2D extent_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;
};
