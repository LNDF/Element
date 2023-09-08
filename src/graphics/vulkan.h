#pragma once

#include <cinttypes>
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <vector>
#include <functional>

#define ELM_REGISTER_VULKAN_REQUIRED_EXTENSION_HOOK(hook) static bool __elm_register_required_extension_hook_##hook = element::vulkan::preregister_required_extension_hook(hook)

namespace element {

    namespace vulkan {
        struct physical_device_info {
            std::unordered_set<std::string> supported_extensions;
            std::unordered_set<std::string> supported_layers;
            std::uint32_t graphics_queue_index;
            std::uint32_t present_queue_index;
            bool supported;
        };

        struct swapchain_support_info {
            std::uint32_t min_image_count, max_image_count;
            std::uint32_t min_extent_width, min_extent_height;
            std::uint32_t max_extent_width, max_extent_height;
            std::uint32_t extent_width, extent_height;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> present_modes;

        };

        struct swapchain_creation_info {
            swapchain_support_info support;
            vk::SurfaceKHR surface;
            std::uint32_t width, height;
            std::uint32_t image_count;
            vk::SurfaceFormatKHR format;
            vk::PresentModeKHR present_mode;
            vk::SurfaceTransformFlagBitsKHR pre_transform;
            vk::CompositeAlphaFlagBitsKHR composite_alpha;
            bool clip;
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
        extern bool __vulkan_device_created;
#ifdef ELM_ENABLE_LOGGING
        extern vk::DebugUtilsMessengerEXT __vulkan_debug_messenger;
#endif
    } // namespace __detail

    namespace vulkan {
        physical_device_info get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions, const std::vector<const char*>& required_layers);
        std::uint32_t pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices);

        void init_instance();
        void init_device(vk::SurfaceKHR& surface);
        swapchain_creation_info query_swapchain_info(vk::SurfaceKHR& surface, std::uint32_t width, std::uint32_t height);
        vk::SwapchainKHR create_swapchain(swapchain_creation_info& info);
        void cleanup();

        std::vector<std::function<void(std::vector<const char*>&)>>& get_preregistered_required_extension_hook();
        bool preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook);

        inline std::uint32_t get_version() {return __detail::__vulkan_version;}
        inline vk::Instance get_instance() {return __detail::__vulkan_instance;}
        inline vk::DispatchLoaderDynamic get_dld() {return __detail::__vulkan_dld;}
        inline vk::PhysicalDevice get_physical_device() {return __detail::__vulkan_physical_device;}
        inline vk::Device get_device() {return __detail::__vulkan_device;}
        inline bool is_device_created() {return __detail::__vulkan_device_created;}
        inline const std::unordered_set<std::string>& get_supported_instance_extensions() {return __detail::__vulkan_supported_instance_extensions;}
        inline const std::unordered_set<std::string>& get_supported_instance_layers() {return __detail::__vulkan_supported_instance_layers;}
        inline const physical_device_info& get_physical_device_info() {return __detail::__vulkan_physical_device_info;}
    } // namespace vulkan

} // namespace element
