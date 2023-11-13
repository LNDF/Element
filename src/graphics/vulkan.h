#pragma once

#define VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC

#include <cinttypes>
#include <vulkan/vulkan.hpp>
#include <utils/pre_exec.h>

#include <unordered_set>
#include <vector>
#include <functional>

#define ELM_REGISTER_VULKAN_REQUIRED_EXTENSION_HOOK(hook) ELM_PRE_EXECUTE(element::__detail::__vulkan_preregister_required_extension_hook, hook);

namespace element {

    namespace __detail {
        std::vector<std::function<void(std::vector<const char*>&)>>& __vulkan_get_preregistered_required_extension_hook();
        bool __vulkan_preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook);
    } // namespace __detail

    namespace vulkan {
        struct physical_device_info_type {
            std::unordered_set<std::string> supported_extensions;
            std::unordered_set<std::string> supported_layers;
            std::uint32_t graphics_queue_index;
            std::uint32_t present_queue_index;
            bool supported;
        };

        extern std::unordered_set<std::string> supported_instance_extensions;
        extern std::unordered_set<std::string> supported_instance_layers;
        extern element::vulkan::physical_device_info_type physical_device_info;
        extern std::uint32_t version;

        extern bool device_initialized;
        extern vk::Instance instance;
        extern vk::DispatchLoaderDynamic dld;
        extern vk::PhysicalDevice physical_device;
        extern vk::Device device;
        extern vk::CommandPool command_pool;
        extern vk::Queue graphics_queue;
        extern vk::Queue present_queue;
#ifdef ELM_ENABLE_LOGGING
        extern vk::DebugUtilsMessengerEXT debug_messenger;
#endif

    } // namespace vulkan

    namespace vulkan {
        physical_device_info_type get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions, const std::vector<const char*>& required_layers);
        std::uint32_t pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices);

        void init_instance();
        void init_device(vk::SurfaceKHR& surface);
        void cleanup();
    } // namespace vulkan

} // namespace element
