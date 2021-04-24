#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    inline bool is_complete() const
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

class PhysicalDevice
{
public:
    static const std::vector<const char*> device_ext;

    void create();
    void destroy();

    inline VkPhysicalDevice handle() const
    {
        return handle_;
    }

    inline operator VkPhysicalDevice() const
    {
        return handle_;
    }

    void check_device_ok() const;
    void check_ext_support() const;
    QueueFamilyIndices get_queue_family_indices() const;
    SwapChainSupportDetails get_swapchain_support_details() const;

private:
    VkPhysicalDevice handle_;
    VkPhysicalDeviceProperties props_;
};
