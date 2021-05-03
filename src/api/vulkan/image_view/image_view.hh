#pragma once

#include <vulkan/vulkan.h>

class ImageView
{
public:
    void create(VkImage image, VkImageType image_type, VkFormat image_format, VkImageAspectFlags image_aspect);
    void destroy();

    inline const VkImageView& handle() const
    {
        return handle_;
    }

    inline operator const VkImageView&() const
    {
        return handle_;
    }

private:
    VkImageView handle_;
};
