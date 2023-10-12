#include "context.h"

#include <vector>

#include "vk/vk_tools.h"

#include "bul/log.h"
#include "bul/containers/static_vector.h"

namespace vk
{
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT msg_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*)
{
    if (msg_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        return VK_FALSE;
    }

    const auto log_level = [msg_severity]() {
        switch (msg_severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return bul::LogLevel::Debug;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return bul::LogLevel::Info;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return bul::LogLevel::Warning;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return bul::LogLevel::Error;
        default:
            return bul::LogLevel::Count;
        }
    }();

    const auto msg_type_str = [msg_type]() {
        switch (msg_type)
        {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            return "GENERAL";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            return "VALIDATION";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            return "PERFORMANCE";
        default:
            return "UNKNOWN TYPE";
        }
    }();

    bul::log(log_level, "VULKAN [%s]: %s", msg_type_str, callback_data->pMessage);

    return VK_FALSE;
}

static void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks* pAllocator,
                                             VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static void create_physical_device(context* context)
{
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, nullptr);
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices.data());

    if (physical_devices.empty())
    {
        bul::log_error("No physical device found");
        abort();
    }

    for (size_t i = 0; i < physical_devices.size(); ++i)
    {
        context->physical_device = physical_devices[i];
        vkGetPhysicalDeviceProperties(context->physical_device, &context->physical_device_properties);
        if (context->physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            break;
        }
    }

    bul::log_info("Physical device: %s", context->physical_device_properties.deviceName);
}

static void create_device(context* context)
{
    bul::static_vector<const char*, 1> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_count, queue_families.data());

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    float priority = 1.0;

    for (uint32_t i = 0; i < queue_families.size(); i++)
    {
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &priority;

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && context->graphics_queue_index == UINT32_MAX)
        {
            context->graphics_queue_index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT && context->compute_queue_index == UINT32_MAX)
        {
            context->compute_queue_index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT && context->transfer_queue_index == UINT32_MAX)
        {
            context->transfer_queue_index = i;
        }
        else
        {
            continue;
        }
        queue_create_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures2 vulkan_features{};
    vulkan_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vulkan_features.features.shaderInt64 = true;

    VkPhysicalDeviceVulkan12Features vulkan12_features{};
    vulkan12_features.pNext = &vulkan_features;
    vulkan12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vulkan12_features.bufferDeviceAddress = true;
    vulkan12_features.descriptorIndexing = true;
    vulkan12_features.runtimeDescriptorArray = true;
    vulkan12_features.descriptorBindingPartiallyBound = true;
    vulkan12_features.shaderSampledImageArrayNonUniformIndexing = true;
    vulkan12_features.descriptorBindingSampledImageUpdateAfterBind = true;

    VkPhysicalDeviceVulkan13Features vulkan13_features{};
    vulkan13_features.pNext = &vulkan12_features;
    vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vulkan13_features.synchronization2 = true;
    vulkan13_features.dynamicRendering = true;

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = &vulkan13_features;
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.queueCreateInfoCount = queue_create_infos.size();
    device_create_info.enabledExtensionCount = device_extensions.size();
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    device_create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(context->physical_device, &device_create_info, nullptr, &context->device));
    volkLoadDevice(context->device);

    constexpr uint32_t descriptor_count = 256;
    bul::static_vector<VkDescriptorPoolSize, 4> pool_sizes{
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = descriptor_count},
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = descriptor_count;

    vkCreateDescriptorPool(context->device, &pool_info, nullptr, &context->descriptor_pool);

    VmaVulkanFunctions vma_functions = {};
    vma_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vma_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.instance = context->instance;
    allocator_info.physicalDevice = context->physical_device;
    allocator_info.device = context->device;
    allocator_info.pVulkanFunctions = &vma_functions;
    VK_CHECK(vmaCreateAllocator(&allocator_info, &context->allocator));

    vkGetDeviceQueue(context->device, context->graphics_queue_index, 0, &context->graphics_queue);
    vkGetDeviceQueue(context->device, context->compute_queue_index, 0, &context->compute_queue);
    vkGetDeviceQueue(context->device, context->transfer_queue_index, 0, &context->transfer_queue);
}

context context::create(bool enable_validation)
{
    bul::static_vector<const char*, 1> validation_layers = {"VK_LAYER_KHRONOS_validation"};
    bul::static_vector<const char*, 3> instance_extensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                                                             VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

    VK_CHECK(volkInitialize());

    context context;

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "vk_graphics_stuff";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "vk_graphics_stuff";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    if (enable_validation)
    {
        instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = instance_extensions.size();
    instance_create_info.ppEnabledExtensionNames = instance_extensions.data();
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.pNext = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation)
    {
        instance_create_info.enabledLayerCount = validation_layers.size();
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
        populate_debug_messenger_create_info(debug_create_info);
        instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &context.instance));
    volkLoadInstance(context.instance);

    if (enable_validation)
    {
        // VkDebugUtilsMessengerCreateInfoEXT create_info;
        // populate_debug_messenger_create_info(create_info);
        VK_CHECK(CreateDebugUtilsMessengerEXT(context.instance, &debug_create_info, nullptr, &context.debug_messenger));
    }

    create_physical_device(&context);
    create_device(&context);

    return context;
}

void context::destroy()
{
    vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
    descriptor_pool = VK_NULL_HANDLE;
    vmaDestroyAllocator(allocator);
    allocator = VK_NULL_HANDLE;
    vkDestroyDevice(device, nullptr);
    device = VK_NULL_HANDLE;

    if (debug_messenger != VK_NULL_HANDLE)
    {
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}
} // namespace vk
