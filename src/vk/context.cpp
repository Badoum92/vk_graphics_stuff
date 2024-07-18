#include "context.h"

#include "vk/vk_tools.h"

#include "bul/log.h"
#include "bul/containers/static_vector.h"

namespace vk
{
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT msg_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*)
{
    const auto log_level = [](VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity) {
        switch (msg_severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return bul::log_level::debug;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return bul::log_level::info;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return bul::log_level::warning;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return bul::log_level::error;
        default:
            return bul::log_level::_count;
        }
    }(msg_severity);

    const auto msg_type_str = [](VkDebugUtilsMessageTypeFlagsEXT msg_type) {
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
    }(msg_type);

    bul::log(log_level, "VULKAN [%s]: %s", msg_type_str, callback_data->pMessage);

    return VK_FALSE;
}

static void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    // clang-format off
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    // create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    // create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr;
    // clang-format on
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

static void create_instance(context* context, bool enable_validation)
{
    bul::static_vector<const char*, 1> validation_layers;
    validation_layers.push_back("VK_LAYER_KHRONOS_validation");
    bul::static_vector<const char*, 16> instance_extensions;
    instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    VK_CHECK(volkInitialize());

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "vk_graphics_stuff";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "vk_graphics_stuff";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    if (enable_validation)
    {
        instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledExtensionCount = instance_extensions.size;
    instance_create_info.ppEnabledExtensionNames = instance_extensions.data;
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.pNext = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation)
    {
        instance_create_info.enabledLayerCount = validation_layers.size;
        instance_create_info.ppEnabledLayerNames = validation_layers.data;
        populate_debug_messenger_create_info(debug_create_info);
        instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    VK_CHECK(vkCreateInstance(&instance_create_info, nullptr, &context->instance));
    volkLoadInstance(context->instance);

    if (enable_validation)
    {
        VK_CHECK(
            CreateDebugUtilsMessengerEXT(context->instance, &debug_create_info, nullptr, &context->debug_messenger));
    }
}

static void create_physical_device(context* context)
{
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, nullptr);
    bul::static_vector<VkPhysicalDevice, 4> physical_devices;
    physical_devices.resize(physical_device_count);
    vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices.data);

    ASSERT(physical_devices.size != 0, "No physical device found");

    for (uint32_t i = 0; i < physical_devices.size; ++i)
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
    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
    };

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_count, nullptr);
    bul::static_vector<VkQueueFamilyProperties, 8> queue_families;
    queue_families.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_count, queue_families.data);

    bul::static_vector<VkDeviceQueueCreateInfo, 8> queue_create_infos;
    float priority = 1.0;

    for (uint32_t i = 0; i < queue_families.size; i++)
    {
        VkDeviceQueueCreateInfo queue_info = {};
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

    VkPhysicalDeviceFeatures2 vulkan2_features = {};
    vulkan2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vulkan2_features.features.shaderInt64 = true;

    VkPhysicalDeviceVulkan12Features vulkan12_features = {};
    vulkan12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vulkan12_features.pNext = &vulkan2_features;
    vulkan12_features.bufferDeviceAddress = true;
    vulkan12_features.descriptorIndexing = true;
    vulkan12_features.runtimeDescriptorArray = true;
    vulkan12_features.descriptorBindingPartiallyBound = true;
    vulkan12_features.shaderSampledImageArrayNonUniformIndexing = true;
    vulkan12_features.descriptorBindingVariableDescriptorCount = true;
    vulkan12_features.descriptorBindingSampledImageUpdateAfterBind = true;
    vulkan12_features.descriptorBindingUpdateUnusedWhilePending = true;
    vulkan12_features.descriptorBindingStorageImageUpdateAfterBind = true;

    VkPhysicalDeviceVulkan13Features vulkan13_features = {};
    vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vulkan13_features.pNext = &vulkan12_features;
    vulkan13_features.synchronization2 = true;
    vulkan13_features.dynamicRendering = true;

    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptor_buffer_feature = {};
    descriptor_buffer_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    descriptor_buffer_feature.pNext = &vulkan13_features;
    descriptor_buffer_feature.descriptorBuffer = true;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = &descriptor_buffer_feature;
    device_create_info.pQueueCreateInfos = queue_create_infos.data;
    device_create_info.queueCreateInfoCount = queue_create_infos.size;
    device_create_info.enabledExtensionCount = BUL_ARRAY_SIZE(device_extensions);
    device_create_info.ppEnabledExtensionNames = device_extensions;
    device_create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(context->physical_device, &device_create_info, nullptr, &context->device));
    volkLoadDevice(context->device);

    VmaVulkanFunctions vma_functions = {};
    vma_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vma_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;
    allocator_info.instance = context->instance;
    allocator_info.physicalDevice = context->physical_device;
    allocator_info.device = context->device;
    allocator_info.pVulkanFunctions = &vma_functions;
    allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    VK_CHECK(vmaCreateAllocator(&allocator_info, &context->allocator));

    vkGetDeviceQueue(context->device, context->graphics_queue_index, 0, &context->graphics_queue);
    vkGetDeviceQueue(context->device, context->compute_queue_index, 0, &context->compute_queue);
    vkGetDeviceQueue(context->device, context->transfer_queue_index, 0, &context->transfer_queue);

    context->descriptor_buffer_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
    context->descriptor_buffer_properties.pNext = nullptr;
    VkPhysicalDeviceProperties2 device_properties = {};
    device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    device_properties.pNext = &context->descriptor_buffer_properties;
    vkGetPhysicalDeviceProperties2(context->physical_device, &device_properties);

    size_t image_sampler_size = context->descriptor_buffer_properties.combinedImageSamplerDescriptorSize;
    size_t storage_image_size = context->descriptor_buffer_properties.storageImageDescriptorSize;
    ENSURE(image_sampler_size == storage_image_size);
}

static void create_frame_contexts(context* context)
{
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = 0;

    for (frame_context& fc : context->frame_contexts)
    {
        VK_CHECK(vkCreateSemaphore(context->device, &semaphore_info, nullptr, &fc.image_acquired_semaphore));
        VK_CHECK(vkCreateSemaphore(context->device, &semaphore_info, nullptr, &fc.rendering_finished_semaphore));
        VK_CHECK(vkCreateFence(context->device, &fence_info, nullptr, &fc.rendering_finished_fence));
        fc.graphics_commands = command_pool::create(context, context->graphics_queue_index, context->graphics_queue);
        fc.compute_commands = command_pool::create(context, context->compute_queue_index, context->compute_queue);
    }
}

context context::create(bul::window* _window, bool enable_validation)
{
    context context;
    context.window = _window;
    create_instance(&context, enable_validation);
    create_physical_device(&context);
    create_device(&context);
    context.surface = surface::create(&context);
    context.descriptor_set = descriptor_set::create(&context);
    context.transfer_commands = command_pool::create(&context, context.graphics_queue_index, context.graphics_queue);
    create_frame_contexts(&context);

    sampler_description sampler_description = {};
    context.default_sampler = context.create_sampler(sampler_description);

    return context;
}

void context::destroy()
{
    destroy_image(undefined_image_handle);
    destroy_sampler(default_sampler);

    for (auto& fc : frame_contexts)
    {
        vkDestroySemaphore(device, fc.image_acquired_semaphore, nullptr);
        vkDestroySemaphore(device, fc.rendering_finished_semaphore, nullptr);
        vkDestroyFence(device, fc.rendering_finished_fence, nullptr);
        fc.graphics_commands.destroy();
        fc.compute_commands.destroy();
    }

    transfer_commands.destroy();
    descriptor_set.destroy(this);
    surface.destroy(this);

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

void context::wait_idle()
{
    VK_CHECK(vkDeviceWaitIdle(device));
}

frame_context* context::get_frame_context()
{
    return &frame_contexts[current_frame];
}

frame_context* context::acquire_next_image()
{
    frame_context& frame_context = frame_contexts[current_frame];
    vkWaitForFences(device, 1, &frame_context.rendering_finished_fence, VK_TRUE, UINT64_MAX);
    VkResult res = vkAcquireNextImageKHR(device, surface.swapchain, UINT64_MAX, frame_context.image_acquired_semaphore,
                                         VK_NULL_HANDLE, &frame_context.image_index);

    frame_context.graphics_commands.reset();
    frame_context.compute_commands.reset();
    frame_context.image = surface.images[frame_context.image_index];

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        wait_idle();
        surface.destroy_swapchain(this);
        surface.create_swapchain(this);
        return nullptr;
    }

    VK_CHECK(res);

    return &frame_context;
}

bool context::present(frame_context* frame_context)
{
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &frame_context->rendering_finished_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &surface.swapchain;
    present_info.pImageIndices = &frame_context->image_index;

    auto res = vkQueuePresentKHR(graphics_queue, &present_info);

    current_frame = (current_frame + 1) % max_frames_in_flight;

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        wait_idle();
        surface.destroy_swapchain(this);
        surface.create_swapchain(this);
        return false;
    }

    VK_CHECK(res);

    return true;
}

void context::submit(command_buffer* command_buffer)
{
    command_buffer->end();
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->vk_handle;
    submit_info.pWaitDstStageMask = &command_buffer->wait_stage;
    VK_CHECK(vkQueueSubmit(command_buffer->vk_queue, 1, &submit_info, VK_NULL_HANDLE));
}

void context::submit(command_buffer* command_buffer, frame_context* frame_context)
{
    command_buffer->end();
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->vk_handle;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &frame_context->image_acquired_semaphore;
    submit_info.pWaitDstStageMask = &command_buffer->wait_stage;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &frame_context->rendering_finished_semaphore;
    vkResetFences(device, 1, &frame_context->rendering_finished_fence);
    VK_CHECK(vkQueueSubmit(command_buffer->vk_queue, 1, &submit_info, frame_context->rendering_finished_fence));
}
} // namespace vk
