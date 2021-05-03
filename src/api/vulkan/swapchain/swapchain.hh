#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "image_view/image_view.hh"

class SwapChain
{
public:
    void create();
    void destroy();

    const VkFormat& format() const;
    const VkExtent2D& extent() const;
    const std::vector<ImageView>& image_views() const;
    const VkViewport& default_viewport() const;
    const VkRect2D& default_scissor() const;
    size_t size() const;

    static void set_surface_capabilities(const VkSurfaceCapabilitiesKHR& capabilites);
    static void choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
    static void choose_swap_present_mode(const std::vector<VkPresentModeKHR>& present_modes);
    void choose_swap_extent();

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

    static VkPresentModeKHR present_mode_;
    static VkSurfaceFormatKHR surface_format_;
    VkSurfaceCapabilitiesKHR surface_capabilities_;
    VkExtent2D extent_;

    std::vector<ImageView> image_views_;

    VkViewport default_viewport_;
    VkRect2D default_scissor_;
};
