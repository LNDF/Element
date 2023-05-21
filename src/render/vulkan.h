#pragma once

#include <cinttypes>
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <vector>
#include <functional>

#define ELM_REGISTER_VULKAN_REQUIRED_EXTENSION_HOOK(hook) static bool __elm_register_required_extension_hook_##hook = element::vulkan::preregister_required_extension_hook(hook)

namespace element {

    struct vulkan_physical_device_info {
        std::unordered_set<std::string> supported_extensions;
        std::uint32_t graphics_queue_index;
        std::uint32_t present_queue_index;
        bool supported;
    };

    class vulkan {
        private:
            static std::unordered_set<std::string> supported_instance_extensions;
            static std::unordered_set<std::string> supported_instance_layers;
            static std::unordered_set<std::string> supported_device_extensions;
            static std::uint32_t version;

            static vk::Instance instance;
            static vk::DispatchLoaderDynamic dld;
            static vk::PhysicalDevice physical_device;
            static vk::Device device;

            static bool device_created;
#ifdef ELM_ENABLE_LOGGING
            static vk::DebugUtilsMessengerEXT debug_messenger;
#endif

        public:
            static vulkan_physical_device_info get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions);
            static std::uint32_t pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices);

            static void init_instance();
            static void init_device(vk::SurfaceKHR& surface);
            static void cleanup();

            static std::vector<std::function<void(std::vector<const char*>&)>>& get_preregistered_required_extension_hook();
            static bool preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook);

            static inline std::uint32_t get_version() {return version;}
            static inline vk::Instance get_instance() {return instance;}
            static inline vk::DispatchLoaderDynamic get_dld() {return dld;}
            static inline vk::PhysicalDevice get_physical_device() {return physical_device;}
            static inline vk::Device get_device() {return device;}
            static inline bool is_device_created() {return device_created;}
            static inline const std::unordered_set<std::string>& get_supported_instance_extensions() {return supported_instance_extensions;}
            static inline const std::unordered_set<std::string>& get_supported_instance_layers() {return supported_instance_layers;}
            static inline const std::unordered_set<std::string>& get_supported_device_extensions() {return supported_device_extensions;}
    };

} // namespace element
