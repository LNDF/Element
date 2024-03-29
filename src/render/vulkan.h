#pragma once

#define VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC
#define VMA_VULKAN_VERSION 1000000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

//See https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/commit/383f06b9fd9d36fb54cb3d4ac3363a670104bc62
#include <cstdio>

#include <cinttypes>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <utils/pre_exec.h>

#include <unordered_set>
#include <vector>
#include <functional>

namespace element {

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
        extern VmaAllocator allocator;
        extern vk::CommandPool command_pool;
        extern vk::Queue graphics_queue;
        extern vk::Queue present_queue;
#ifdef ELM_ENABLE_LOGGING
        extern vk::DebugUtilsMessengerEXT debug_messenger;
#endif

        physical_device_info_type get_physical_device_support(vk::PhysicalDevice& device, vk::SurfaceKHR& surface, const std::vector<const char*>& required_extensions, const std::vector<const char*>& required_layers);
        std::uint32_t pick_best_physical_device(const std::vector<vk::PhysicalDevice>& devices);
        void add_instance_extensions(const char** extensions, std::uint32_t count);

        void init_instance();
        void init_device(vk::SurfaceKHR& surface);
        void cleanup();
    } // namespace vulkan

} // namespace element
