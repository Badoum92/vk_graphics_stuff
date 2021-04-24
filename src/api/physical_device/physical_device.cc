#include "physical_device.hh"

#include <iostream>
#include <stdexcept>
#include <unordered_set>

#include "vk_context/vk_context.hh"

const std::vector<const char*> PhysicalDevice::device_ext = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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

    auto details = get_swapchain_support_details();
    if (details.formats.empty() || details.present_modes.empty())
    {
        throw std::runtime_error("Incomplete swapchain support");
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

    for (const auto& ext : required_ext)
    {
        std::cerr << "Unsupported device extension: " << ext << "\n";
    }
    exit(1);
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

SwapChainSupportDetails PhysicalDevice::get_swapchain_support_details() const
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle_, VkContext::surface.handle(), &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle_, VkContext::surface.handle(), &format_count, nullptr);
    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle_, VkContext::surface.handle(), &format_count,
                                             details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle_, VkContext::surface.handle(), &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle_, VkContext::surface.handle(), &present_mode_count,
                                                  details.present_modes.data());
    }

    return details;
}
