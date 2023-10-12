#pragma once

#include <cinttypes>
#include <vulkan/vulkan.hpp>
#include <utils/pre_exec.h>

#include <unordered_set>
#include <vector>
#include <functional>

#define ELM_REGISTER_VULKAN_REQUIRED_EXTENSION_HOOK(hook) ELM_PRE_EXECUTE(element::__detail::__vulkan_preregister_required_extension_hook, hook);

namespace element {

    namespace vulkan {
        struct physical_device_info {
            std::unordered_set<std::string> supported_extensions;
            std::unordered_set<std::string> supported_layers;
            std::uint32_t graphics_queue_index;
            std::uint32_t present_queue_index;
            bool supported;
        };
    } // namespace vulkan
    

    namespace __detail {
        extern std::unordered_set<std::string> __vulkan_supported_instance_extensions;
        extern std::unordered_set<std::string> __vulkan_supported_instance_layers;
        extern element::vulkan::physical_device_info __vulkan_physical_device_info;
        extern std::uint32_t __vulkan_version;

        extern vk::Instance __vulkan_instance;
        extern vk::DispatchLoaderDynamic __vulkan_dld;
        extern vk::PhysicalDevice __vulkan_physical_device;
        extern vk::Device __vulkan_device;
        extern vk::CommandPool __vulkan_command_pool;
        extern bool __vulkan_device_created;
#ifdef ELM_ENABLE_LOGGING
        extern vk::DebugUtilsMessengerEXT __vulkan_debug_messenger;
#endif

        std::vector<std::function<void(std::vector<const char*>&)>>& __vulkan_get_preregistered_required_extension_hook();
        bool __vulkan_preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook);
    } // namespace __detail

    namespace vulkan {
        physical_device_info get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions, const std::vector<const char*>& required_layers);
        std::uint32_t pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices);

        void init_instance();
        void init_device(vk::SurfaceKHR& surface);
        void cleanup();

        inline std::uint32_t get_version() {return __detail::__vulkan_version;}
        inline vk::Instance get_instance() {return __detail::__vulkan_instance;}
        inline vk::DispatchLoaderDynamic get_dld() {return __detail::__vulkan_dld;}
        inline vk::PhysicalDevice get_physical_device() {return __detail::__vulkan_physical_device;}
        inline vk::Device get_device() {return __detail::__vulkan_device;}
        inline vk::CommandPool get_command_pool() {return __detail::__vulkan_command_pool;}
        inline bool is_device_created() {return __detail::__vulkan_device_created;}
        inline const std::unordered_set<std::string>& get_supported_instance_extensions() {return __detail::__vulkan_supported_instance_extensions;}
        inline const std::unordered_set<std::string>& get_supported_instance_layers() {return __detail::__vulkan_supported_instance_layers;}
        inline const physical_device_info& get_physical_device_info() {return __detail::__vulkan_physical_device_info;}
    } // namespace vulkan

} // namespace element
