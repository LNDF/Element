#include "vulkan.h"

#include <core/engine.h>
#include <core/log.h>

using namespace element;

std::uint32_t vulkan::version = 0;
std::unordered_set<std::string> vulkan::supported_instance_extensions;
std::unordered_set<std::string> vulkan::supported_instance_layers;
vk::Instance vulkan::instance;
vk::DispatchLoaderDynamic vulkan::dld;
#ifdef ELM_ENABLE_LOGGING
vk::DebugUtilsMessengerEXT vulkan::debug_messenger;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_logger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data) {
    ELM_DEBUG("vk: {}", data->pMessage);
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
}

void vulkan::cleanup() {
    ELM_INFO("Cleanning up Vulkan...");
#ifdef ELM_ENABLE_LOGGING
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