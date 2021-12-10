#include "context.hh"

#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

#include "vk_tools.hh"

namespace vk
{
static const bool enable_validation_layers = true;
static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

static void check_validation_layer_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char* layer_name : validation_layers)
    {
        bool found = false;
        for (const auto& layer_prop : available_layers)
        {
            if (strcmp(layer_name, layer_prop.layerName) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::string err = "Unsupported validation layer: ";
            err += layer_name;
            throw std::runtime_error(err);
        }
    }
}

static std::vector<const char*> get_required_ext()
{
    uint32_t glfw_ext_count = 0;
    const char** glfw_ext;
    glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

    std::vector<const char*> ext(glfw_ext, glfw_ext + glfw_ext_count);

    if (enable_validation_layers)
    {
        ext.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return ext;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT msg_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*)
{
    // if (msg_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    if (msg_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        return VK_FALSE;


    auto const severity_str = [msg_severity]() {
        switch (msg_severity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return "VERBOSE";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return "INFO";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return "WARNING";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return "ERROR";
        default:
            return "UNKNOWN SEVERITY";
        }
    }();

    auto const type_str = [msg_type]() {
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

    std::cerr << "VK_DEBUG [" << severity_str << "][" << type_str << "]: " << callback_data->pMessage << "\n";

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

static void setup_debug_messenger(Context& context)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);
    VK_CHECK(CreateDebugUtilsMessengerEXT(context.instance, &create_info, nullptr, &context.debug_messenger));
}

static std::vector<VkPhysicalDevice> get_physical_devices(const Context& context)
{
    uint32_t physical_device_count;
    vkEnumeratePhysicalDevices(context.instance, &physical_device_count, nullptr);
    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices.data());
    return physical_devices;
}

Context Context::create()
{
    Context context;

    if (enable_validation_layers)
    {
        check_validation_layer_support();
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "vk_graphics_stuff";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "vk_engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    auto ext = get_required_ext();

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = ext.size();
    create_info.ppEnabledExtensionNames = ext.data();
    create_info.enabledLayerCount = 0;
    create_info.pNext = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation_layers)
    {
        create_info.enabledLayerCount = validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();
        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    VK_CHECK(vkCreateInstance(&create_info, nullptr, &context.instance));

    if (enable_validation_layers)
    {
        setup_debug_messenger(context);
    }

    auto devices = get_physical_devices(context);

    if (devices.empty())
    {
        throw std::runtime_error("No physical devices found");
    }

    for (size_t i = 0; i < devices.size(); ++i)
    {
        context.physical_device.vk_handle = devices[i];
        vkGetPhysicalDeviceProperties(context.physical_device.vk_handle, &context.physical_device.properties);

        if (context.physical_device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            break;
        }
    }

    std::cout << "Physical device: " << context.physical_device.properties.deviceName << "\n";

    return context;
}

void Context::destroy()
{
    if (debug_messenger != VK_NULL_HANDLE)
    {
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
}
} // namespace vk
