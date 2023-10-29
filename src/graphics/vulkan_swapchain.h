#pragma once

#include <cinttypes>
#include <vulkan/vulkan.hpp>

#include <vector>

namespace element {

    namespace vulkan {
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

        struct swapchain_info {
            vk::SwapchainKHR swapchain;
            vk::SurfaceFormatKHR format;
            std::vector<vk::Image> images;
            std::uint32_t width, height;
        };

        swapchain_creation_info query_swapchain_info(vk::SurfaceKHR& surface, std::uint32_t width, std::uint32_t height);
        swapchain_info create_swapchain(swapchain_creation_info& info);
        void destroy_swapchain(swapchain_info& info);

    } // namespace vulkan 

} // namespace element
