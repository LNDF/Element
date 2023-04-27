#include "vulkan.h"

#include <stdexcept>

#include <core/engine.h>
#include <core/log.h>

using namespace element;

std::uint32_t vulkan::version = 0;
std::unordered_set<std::string> vulkan::supported_instance_extensions;
std::unordered_set<std::string> vulkan::supported_instance_layers;
std::unordered_set<std::string> vulkan::supported_device_extensions;
vk::Instance vulkan::instance;
vk::DispatchLoaderDynamic vulkan::dld;
vk::PhysicalDevice vulkan::physical_device;
vk::Device vulkan::device;
#ifdef ELM_ENABLE_LOGGING
vk::DebugUtilsMessengerEXT vulkan::debug_messenger;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_logger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data) {
    log::log_level level;
    const char* type_str = "Vulkan";
    if (severity & VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        level = ELM_LOG_LEVEL_ERROR;
    } else if (severity & VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        level = ELM_LOG_LEVEL_INFO;
    } else if (severity & VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        level = ELM_LOG_LEVEL_TRACE;
    } else if (severity & VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        level = ELM_LOG_LEVEL_WARN;
    } else {
        level = ELM_LOG_LEVEL_DEBUG;
    }
    if (type & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        type_str = "Device address binding";
    } else if (type & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type_str = "General";
    } else if (type & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type_str = "Performance";
    } else if (type & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type_str = "Validation";
    }
    ELM_LOG(level, "[{0}]: {1}", type_str, data->pMessage);
    return VK_FALSE;
}



void vulkan::init() {
    ELM_INFO("Starting Vulkan..");
    version = vk::enumerateInstanceVersion();
    std::vector<vk::ExtensionProperties> supportedInstanceExtensions = vk::enumerateInstanceExtensionProperties();
    std::vector<vk::LayerProperties> supportedInstanceLayers = vk::enumerateInstanceLayerProperties();
    ELM_DEBUG("Max supported Vulkan version: {0}.{1}.{2}", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));
    ELM_DEBUG("Supported instance extensions:");
    for (const vk::ExtensionProperties& properties : supportedInstanceExtensions) {
        ELM_DEBUG("    {0}", properties.extensionName.data());
        supported_instance_extensions.insert(properties.extensionName.data());
    }
    ELM_DEBUG("Supported instance layers:");
    for (const vk::LayerProperties& properties : supportedInstanceLayers) {
        ELM_DEBUG("    {0}", properties.layerName.data());
        supported_instance_layers.insert(properties.layerName.data());
    }
    version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    vk::ApplicationInfo app_info(engine::settings.app_name.c_str(), 0, "element-engine", ELM_VERSION_ID, version);
    std::vector<const char*> extensions;
    std::vector<const char*> layers;
    for (auto& hook : get_preregistered_required_extension_hook()) {
        hook(extensions);
    }
#ifdef ELM_ENABLE_LOGGING
    if (supported_instance_extensions.contains("VK_EXT_debug_utils")) {
        extensions.push_back("VK_EXT_debug_utils");
    } else {
        ELM_WARN("Extension VK_EXT_debug_utils could not be found. Vulkan messages will not be logged.");
    }
#endif
#ifndef NDEBUG
    if (supported_instance_layers.contains("VK_LAYER_KHRONOS_validation")) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    } else {
        ELM_WARN("Vulkan validation layers are not available on this system. Install them to be able to use this feature.");
    }
#endif
    ELM_DEBUG("Using {0} instance extensions:", extensions.size());
    for (const char* ext : extensions) {
        ELM_DEBUG("    {}", ext);
    }
    ELM_DEBUG("Using {0} instance layers:", layers.size());
    for (const char* layer : layers) {
        ELM_DEBUG("    {}", layer);
    }
    vk::InstanceCreateInfo create_info(vk::InstanceCreateFlags(), &app_info, layers.size(), layers.data(), extensions.size(), extensions.data());
    instance = vk::createInstance(create_info);
    dld = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
#ifdef ELM_ENABLE_LOGGING
    if (supported_instance_extensions.contains("VK_EXT_debug_utils")) {
        vk::DebugUtilsMessengerCreateInfoEXT dbg_create_info(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			vulkan_logger,
			nullptr
        );
        debug_messenger = instance.createDebugUtilsMessengerEXT(dbg_create_info, nullptr, dld);
        ELM_DEBUG("Vulkan debug messages will be logged.");
    }
#endif
    bool physical_device_foud = false;
    std::vector<const char*> required_device_extensions;
    required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
    vk::PhysicalDeviceProperties dev_properties;
    ELM_DEBUG("Found {0} physical devices", physical_devices.size());
    ELM_DEBUG("Using {0} device extensions", required_device_extensions.size());
    for (const char* extension : required_device_extensions) {
        ELM_DEBUG("    {}", extension);
    }
    for (vk::PhysicalDevice& dev : physical_devices) {
        supported_device_extensions.clear();
        for (vk::ExtensionProperties& properties : dev.enumerateDeviceExtensionProperties()) {
            supported_device_extensions.insert(properties.extensionName);
        }
        for (const char* extension : required_device_extensions) {
            if (supported_device_extensions.contains(extension)) {
                vk::PhysicalDeviceProperties current_dev_properties = dev.getProperties();
                if (dev_properties.deviceType == vk::PhysicalDeviceType::eOther || !physical_device_foud ||
                    (current_dev_properties.deviceType == vk::PhysicalDeviceType::eCpu &&
                        dev_properties.deviceType == vk::PhysicalDeviceType::eOther) ||
                    (current_dev_properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu &&
                        (dev_properties.deviceType == vk::PhysicalDeviceType::eOther ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eCpu)) ||
                    (current_dev_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu &&
                        (dev_properties.deviceType == vk::PhysicalDeviceType::eOther ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eCpu ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu)) ||
                    (current_dev_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
                        (dev_properties.deviceType == vk::PhysicalDeviceType::eOther ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eCpu ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu ||
                         dev_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu))) {
                    physical_device_foud = true;
                    dev_properties = current_dev_properties;
                    physical_device = dev;
                }
            }
        }
    }
    if (!physical_device_foud) {
        throw std::runtime_error("Couldn't find valid Vulkan physical device");
    }
    const char* dev_type = "Unknown";
    switch (dev_properties.deviceType) {
        case vk::PhysicalDeviceType::eCpu:
            dev_type = "CPU";
            break;
        case vk::PhysicalDeviceType::eDiscreteGpu:
            dev_type = "Discrete GPU";
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            dev_type = "Integrated GPU";
            break;
        case vk::PhysicalDeviceType::eVirtualGpu:
            dev_type = "Virtual GPU";
            break;
        case vk::PhysicalDeviceType::eOther:
            dev_type = "Other";
            break;
        default:
            break;
    }
    ELM_INFO("Selected device properties:");
    ELM_INFO("  Device name: {}", dev_properties.deviceName);
    ELM_INFO("  Device type: {}", dev_type);
    std::vector<vk::QueueFamilyProperties> queue_family_properties = physical_device.getQueueFamilyProperties();
    bool graphics_queue_found = false;
    std::uint32_t graphics_queue_index = 0;
    for (vk::QueueFamilyProperties& properties : queue_family_properties) {
        if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_queue_found = true;
            break;
        }
        graphics_queue_index++;
    }
    if (!graphics_queue_found) {
        throw std::runtime_error("Couldn't find graphics queue family,");
    }
    ELM_DEBUG("Graphics queue family index is {}", graphics_queue_index);
    float queue_priority = 1.0f;
    vk::DeviceQueueCreateInfo queue_create_info(vk::DeviceQueueCreateFlags(), graphics_queue_index, 1, &queue_priority);
    vk::PhysicalDeviceFeatures physical_device_feautres;
    std::vector<const char*> device_layers;
#ifndef NDEBUG
    if (supported_instance_layers.contains("VK_LAYER_KHRONOS_validation")) {
        device_layers.push_back("VK_LAYER_KHRONOS_validation");
    }
#endif
    vk::DeviceCreateInfo device_create_info{vk::DeviceCreateFlags(), 1, &queue_create_info, static_cast<std::uint32_t>(device_layers.size()), device_layers.data(), 0, nullptr, &physical_device_feautres};
    device = physical_device.createDevice(device_create_info);
    ELM_INFO("Vulkan started");
}

void vulkan::cleanup() {
    ELM_INFO("Cleanning up Vulkan...");
#ifdef ELM_ENABLE_LOGGING
    device.destroy();
    physical_device = nullptr;
    instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dld);
#endif
    instance.destroy();
}

std::vector<std::function<void(std::vector<const char*>&)>>& vulkan::get_preregistered_required_extension_hook() {
    static std::vector<std::function<void(std::vector<const char*>&)>> hooks;
    return hooks;
}

bool vulkan::preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook) {
     get_preregistered_required_extension_hook().push_back(hook);
     return true;
}