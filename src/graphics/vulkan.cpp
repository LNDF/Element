#include "vulkan.h"

#include <stdexcept>
#include <limits>

#include <core/engine.h>
#include <core/log.h>
#include <graphics/vulkan_descriptor..h>

using namespace element;

std::uint32_t vulkan::version = 0;
std::unordered_set<std::string> vulkan::supported_instance_extensions;
std::unordered_set<std::string> vulkan::supported_instance_layers;
vulkan::physical_device_info_type vulkan::physical_device_info;
bool vulkan::device_initialized = false;
vk::Instance vulkan::instance;
vk::DispatchLoaderDynamic vulkan::dld;
vk::PhysicalDevice vulkan::physical_device = nullptr;
vk::CommandPool vulkan::command_pool;
vk::Queue vulkan::graphics_queue;
vk::Queue vulkan::present_queue;
vk::Device vulkan::device = nullptr;
#ifdef ELM_ENABLE_LOGGING
vk::DebugUtilsMessengerEXT vulkan::debug_messenger = nullptr;
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

vulkan::physical_device_info_type vulkan::get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions, const std::vector<const char*>& required_layers) {
    physical_device_info_type info;
    info.supported = false;
    info.graphics_queue_index = 0;
    info.present_queue_index = 0;
    std::vector<vk::QueueFamilyProperties> props = device.getQueueFamilyProperties();
    int i = 0, sup = 0;
    for (vk::QueueFamilyProperties& prop : props) {
        if (prop.queueFlags & vk::QueueFlagBits::eGraphics) {
            info.graphics_queue_index = i;
            sup++;
        }
        if (device.getSurfaceSupportKHR(i, surface)) {
            info.present_queue_index = i;
            sup++;
        }
        i++;
        if (sup == 2) {
            info.supported = true;
            break;
        }
    }
    if (!info.supported) return info;
    std::vector<vk::ExtensionProperties> extprops = device.enumerateDeviceExtensionProperties();
    info.supported_extensions.reserve(extprops.size());
    for (vk::ExtensionProperties& props : extprops) {
        info.supported_extensions.insert(props.extensionName.data());
    }
    for (const char* extension : required_extensions) {
        if (!info.supported_extensions.contains(extension)) {
            info.supported = false;
            return info;
        }
    }
    std::vector<vk::LayerProperties> layerprops = device.enumerateDeviceLayerProperties();
    info.supported_layers.reserve(layerprops.size());
    for (vk::LayerProperties props : layerprops) {
        info.supported_layers.insert(props.layerName.data());
    }
    for (const char* layer : required_layers) {
        if (!info.supported_layers.contains(layer)) {
            info.supported = false;
            return info;
        }
    }
    return info;
}

std::uint32_t vulkan::pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices) {
    std::uint32_t index = devices.size(), i = 0;
    vk::PhysicalDeviceProperties dev_props;
    for (const vk::PhysicalDevice& dev : devices) {
        vk::PhysicalDeviceProperties current_dev_props = dev.getProperties();
        if (dev_props.deviceType == vk::PhysicalDeviceType::eOther || index == devices.size() ||
            (current_dev_props.deviceType == vk::PhysicalDeviceType::eCpu &&
             dev_props.deviceType == vk::PhysicalDeviceType::eOther) ||
            (current_dev_props.deviceType == vk::PhysicalDeviceType::eVirtualGpu &&
            (dev_props.deviceType == vk::PhysicalDeviceType::eOther ||
             dev_props.deviceType == vk::PhysicalDeviceType::eCpu)) ||
            (current_dev_props.deviceType == vk::PhysicalDeviceType::eIntegratedGpu &&
            (dev_props.deviceType == vk::PhysicalDeviceType::eOther ||
             dev_props.deviceType == vk::PhysicalDeviceType::eCpu ||
             dev_props.deviceType == vk::PhysicalDeviceType::eVirtualGpu)) ||
            (current_dev_props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
            (dev_props.deviceType == vk::PhysicalDeviceType::eOther ||
             dev_props.deviceType == vk::PhysicalDeviceType::eCpu ||
             dev_props.deviceType == vk::PhysicalDeviceType::eVirtualGpu ||
             dev_props.deviceType == vk::PhysicalDeviceType::eIntegratedGpu))) {
            index = i;
            dev_props = current_dev_props;
        }
        i++;
    }
    return index;
}

void vulkan::init_instance() {
    ELM_INFO("Starting Vulkan instance..");
    version = vk::enumerateInstanceVersion();
    std::vector<vk::ExtensionProperties> supported_instance_extension_props = vk::enumerateInstanceExtensionProperties();
    std::vector<vk::LayerProperties> supported_instance_layer_props = vk::enumerateInstanceLayerProperties();
    ELM_DEBUG("Max supported Vulkan version: {0}.{1}.{2}", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));
    ELM_DEBUG("Supported instance extensions:");
    supported_instance_extensions.reserve(supported_instance_extensions.size());
    for (const vk::ExtensionProperties& properties : supported_instance_extension_props) {
        ELM_DEBUG("    {0}", properties.extensionName.data());
        supported_instance_extensions.insert(properties.extensionName.data());
    }
    ELM_DEBUG("Supported instance layers:");
    supported_instance_layers.reserve(supported_instance_layers.size());
    for (const vk::LayerProperties& properties : supported_instance_layer_props) {
        ELM_DEBUG("    {0}", properties.layerName.data());
        supported_instance_layers.insert(properties.layerName.data());
    }
    version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    vk::ApplicationInfo app_info(engine::settings.app_name.c_str(), 0, "element-engine", ELM_VERSION_ID, version);
    std::vector<const char*> extensions;
    std::vector<const char*> layers;
    for (auto& hook : __detail::__vulkan_get_preregistered_required_extension_hook()) {
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
}

void vulkan::init_device(vk::SurfaceKHR& surface) {
    ELM_INFO("Creating Vulkan device..");
    if (device_initialized) {
        ELM_WARN("Device already created");
        return;
    }
    std::vector<const char*> device_extensions, device_layers;
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
    std::vector<vk::PhysicalDevice> supported_devices;
    std::vector<physical_device_info_type> supported_device_infos;
    ELM_DEBUG("Found {0} physical devices", physical_devices.size());
    supported_devices.reserve(physical_devices.size());
    supported_device_infos.reserve(physical_devices.size());
    for (vk::PhysicalDevice& dev : physical_devices) {
        physical_device_info_type info = get_physical_device_support(dev, surface, device_extensions, device_layers);
        if (info.supported) {
            supported_devices.push_back(dev);
            supported_device_infos.push_back(std::move(info));
        }
    }
    ELM_DEBUG("{0} devices supported", supported_devices.size());
    std::uint32_t device_index = pick_best_physical_device(supported_devices);
    if (device_index == supported_devices.size()) {
        throw std::runtime_error("Couldn't find valid Vulkan physical device");
    }
    physical_device = supported_devices[device_index];
    physical_device_info = std::move(supported_device_infos[device_index]);
    vk::PhysicalDeviceProperties dev_properties = physical_device.getProperties();
    ELM_INFO("Selected device properties:");
    ELM_INFO("  Device name: {}", dev_properties.deviceName);
    ELM_INFO("  Device type: {}", vk::to_string(dev_properties.deviceType).c_str());
    std::vector<std::uint32_t> queue_indices;
    queue_indices.push_back(physical_device_info.graphics_queue_index);
    if (physical_device_info.graphics_queue_index != physical_device_info.present_queue_index) {
        queue_indices.push_back(physical_device_info.present_queue_index);
    }
    ELM_DEBUG("  Graphics queue family: {}", physical_device_info.graphics_queue_index);
    ELM_DEBUG("  Present queue family: {}", physical_device_info.present_queue_index);
    ELM_DEBUG("{} queues will be created", queue_indices.size());
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    float queue_priority = 1.0f;
    for (std::uint32_t index : queue_indices) {
        queue_create_infos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), index, 1, &queue_priority));
    }
#ifndef NDEBUG
    if (supported_instance_layers.contains("VK_LAYER_KHRONOS_validation") && physical_device_info.supported_layers.contains("VK_LAYER_KHRONOS_validation")) {
        device_layers.push_back("VK_LAYER_KHRONOS_validation");
    }
#endif
    ELM_DEBUG("Using {0} device extensions", device_extensions.size());
    for (const char* extension : device_extensions) {
        ELM_DEBUG("    {}", extension);
    }
    ELM_DEBUG("Using {0} device layers", device_layers.size());
    for (const char* layer : device_layers) {
        ELM_DEBUG("    {}", layer);
    }
    vk::PhysicalDeviceFeatures physical_device_feautres;
    vk::DeviceCreateInfo device_create_info{vk::DeviceCreateFlags(), static_cast<std::uint32_t>(queue_create_infos.size()), queue_create_infos.data(), static_cast<std::uint32_t>(device_layers.size()), device_layers.data(), static_cast<std::uint32_t>(device_extensions.size()), device_extensions.data(), &physical_device_feautres};
    device = physical_device.createDevice(device_create_info);
    graphics_queue = device.getQueue(physical_device_info.graphics_queue_index, 0);
    present_queue = device.getQueue(physical_device_info.present_queue_index, 0);
    ELM_INFO("Vulkan device created");
    vk::CommandPoolCreateInfo command_pool_create_info;
    command_pool_create_info.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    command_pool_create_info.queueFamilyIndex = physical_device_info.graphics_queue_index;
    command_pool = device.createCommandPool(command_pool_create_info);
    ELM_DEBUG("Command pool created");
    device_initialized = true;
}

void vulkan::cleanup() {
    ELM_INFO("Cleanning up Vulkan...");
    if (device_initialized) {
        device.waitIdle();
        clean_descriptorsets();
        device.destroyCommandPool(command_pool);
        device.destroy();
        supported_instance_extensions.clear();
        supported_instance_layers.clear();
        device = nullptr;
        physical_device = nullptr;
        device_initialized = false;
    }
#ifdef ELM_ENABLE_LOGGING
    instance.destroyDebugUtilsMessengerEXT(debug_messenger, nullptr, dld);
#endif
    instance.destroy();
}

std::vector<std::function<void(std::vector<const char*>&)>>& __detail::__vulkan_get_preregistered_required_extension_hook() {
    static std::vector<std::function<void(std::vector<const char*>&)>> hooks;
    return hooks;
}

bool __detail::__vulkan_preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook) {
     __vulkan_get_preregistered_required_extension_hook().push_back(hook);
     return true;
}