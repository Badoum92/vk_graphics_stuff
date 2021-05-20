#include "physical_device.hh"

#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include "vk_context/vk_context.hh"
#include "swapchain/swapchain.hh"

const std::vector<const char*> PhysicalDevice::device_ext = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                             VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};

void PhysicalDevice::create()
{
    auto devices = VkContext::instance.get_physical_devices();

    if (devices.empty())
    {
        throw std::runtime_error("No physical devices found");
    }

    handle_ = devices[0];
    vkGetPhysicalDeviceProperties(handle_, &props_);

    if (props_.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        for (size_t i = 1; i < devices.size(); ++i)
        {
            VkPhysicalDevice& next_device = devices[i];
            VkPhysicalDeviceProperties next_props;
            vkGetPhysicalDeviceProperties(next_device, &props_);

            if (next_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                handle_ = next_device;
                props_ = next_props;
                break;
            }
        }
    }

    check_device_ok();
    set_swapchain_support_details();

    std::cout << "Physical device: " << props_.deviceName << "\n";
}

void PhysicalDevice::destroy()
{}

void PhysicalDevice::check_device_ok() const
{
    check_ext_support();

    auto indices = get_queue_family_indices();
    if (!indices.graphics_family.has_value() || !indices.present_family.has_value())
    {
        throw std::runtime_error("Physical device queue families missing");
    }
}

void PhysicalDevice::check_ext_support() const
{
    uint32_t ext_count;
    vkEnumerateDeviceExtensionProperties(handle_, nullptr, &ext_count, nullptr);
    std::vector<VkExtensionProperties> available_ext(ext_count);
    vkEnumerateDeviceExtensionProperties(handle_, nullptr, &ext_count, available_ext.data());

    std::unordered_set<std::string> required_ext(begin(device_ext), end(device_ext));
    for (const auto& ext : available_ext)
    {
        required_ext.erase(ext.extensionName);
    }

    if (required_ext.empty())
        return;

    std::string error_str = "";
    for (const auto& ext : required_ext)
    {
        error_str += " " + ext;
    }
    throw std::runtime_error("Unsupported device extension(s):" + error_str);
}

QueueFamilyIndices PhysicalDevice::get_queue_family_indices() const
{
    QueueFamilyIndices indices;

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_count, queue_families.data());

    uint32_t i = 0;
    for (const auto& queue_family : queue_families)
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(handle_, i, VkContext::surface.handle(), &present_support);

        if (present_support)
        {
            indices.present_family = i;
        }

        ++i;
    }

    return indices;
}

void PhysicalDevice::set_swapchain_support_details() const
{
    uint32_t format_count;
    std::vector<VkSurfaceFormatKHR> formats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle_, VkContext::surface.handle(), &format_count, nullptr);
    if (format_count == 0)
    {
        throw std::runtime_error("No swapchain format supported");
    }
    formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle_, VkContext::surface.handle(), &format_count, formats.data());
    SwapChain::choose_swap_surface_format(formats);

    uint32_t present_mode_count;
    std::vector<VkPresentModeKHR> present_modes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle_, VkContext::surface.handle(), &present_mode_count, nullptr);
    if (present_mode_count == 0)
    {
        throw std::runtime_error("No swapchain present mode supported");
    }
    present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle_, VkContext::surface.handle(), &present_mode_count,
                                              present_modes.data());
    SwapChain::choose_swap_present_mode(present_modes);
}

const VkPhysicalDeviceLimits& PhysicalDevice::get_limits() const
{
    return props_.limits;
}

VkFormat PhysicalDevice::get_supported_format(const std::vector<VkFormat>& formats, VkImageTiling tiling,
                                              VkFormatFeatureFlags features) const
{
    for (VkFormat format : formats)
    {
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(handle_, format, &format_props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (format_props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (format_props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Requested VkFormats not supported");
}
