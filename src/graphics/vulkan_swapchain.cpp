#include "vulkan_swapchain.h"

#include <graphics/vulkan.h>
#include <core/log.h>

using namespace element;

vulkan::swapchain_creation_info vulkan::query_swapchain_info(vk::SurfaceKHR& surface, std::uint32_t width, std::uint32_t height) {
    swapchain_creation_info info;
    vk::SurfaceCapabilitiesKHR capabilities = __detail::__vulkan_physical_device.getSurfaceCapabilitiesKHR(surface);
    info.support.min_image_count = capabilities.minImageCount;
    info.support.max_image_count = capabilities.maxImageCount;
    info.support.min_extent_width = capabilities.minImageExtent.width;
    info.support.min_extent_height = capabilities.minImageExtent.height;
    info.support.max_extent_width = capabilities.maxImageExtent.width;
    info.support.max_extent_height = capabilities.maxImageExtent.height;
    info.support.extent_width = capabilities.currentExtent.width;
    info.support.extent_height = capabilities.currentExtent.height;
    info.support.formats = __detail::__vulkan_physical_device.getSurfaceFormatsKHR(surface);
    info.support.present_modes = __detail::__vulkan_physical_device.getSurfacePresentModesKHR(surface);
    //capabilities.maxImageArrayLayers
    //capabilities.supportedTransforms
    //capabilities.supportedCompositeAlpha
    //capabilities.supportedUsageFlags
    ELM_DEBUG("Query swapchain information:");
    ELM_DEBUG("    Image count: min {0}, max {1}", info.support.min_image_count, info.support.max_image_count);
    ELM_DEBUG("    Image size: min {0}x{1}, max {2}x{3}, current {4}x{5}",
            info.support.min_extent_width, info.support.min_extent_height,
            info.support.max_extent_width, info.support.max_extent_height,
            info.support.extent_width, info.support.extent_height);
    ELM_DEBUG("    Supported surface formats:");
    for (const vk::SurfaceFormatKHR& format : info.support.formats) {
        ELM_DEBUG("      {0} {1}", vk::to_string(format.format).c_str(), vk::to_string(format.colorSpace).c_str());
    }
    ELM_DEBUG("    Supported present modes:");
    for (const vk::PresentModeKHR& present : info.support.present_modes) {
        ELM_DEBUG("      {}", vk::to_string(present).c_str());
    }
    info.surface = surface;
    info.format = info.support.formats[0];
    for (const vk::SurfaceFormatKHR& format : info.support.formats) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            info.format = format;
            break;
        }
    }
    info.present_mode = info.support.present_modes[0];
    for (const vk::PresentModeKHR& present : info.support.present_modes) {
        if (present == vk::PresentModeKHR::eMailbox) {
            info.present_mode = present;
            break;
        }
    }
    if (info.support.extent_width != std::numeric_limits<std::uint32_t>::max()) {
        info.width = info.support.extent_width;
        info.height = info.support.extent_height;
    } else {
        info.width = std::min(info.support.max_extent_width, std::max(info.support.min_extent_width, width));
        info.height = std::min(info.support.max_extent_height, std::max(info.support.min_extent_height, height));
    }
    if (info.support.max_image_count == 0) {
        info.image_count = std::max(info.support.min_image_count, 3u);
    } else {
        info.image_count = std::min(info.support.max_image_count, std::max(info.support.min_image_count, 3u));
    }
    info.pre_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    info.composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    info.clip = true;
    return info;
}

vulkan::swapchain_info vulkan::create_swapchain(vulkan::swapchain_creation_info& info) {
    ELM_INFO("Creating swapchain...");
    ELM_DEBUG("    Image size: {0}x{1}", info.width, info.height);
    ELM_DEBUG("    Image count: {}", info.image_count);
    ELM_DEBUG("    Format: {0} {1}", vk::to_string(info.format.format).c_str(), vk::to_string(info.format.colorSpace).c_str());
    ELM_DEBUG("    Present mode: {}", vk::to_string(info.present_mode).c_str());
    ELM_DEBUG("    Pre transform: {}", vk::to_string(info.pre_transform).c_str());
    ELM_DEBUG("    Composite alpha: {}", vk::to_string(info.composite_alpha).c_str());
    vk::Extent2D extent(info.width, info.height);
    vk::SwapchainCreateInfoKHR create_info = vk::SwapchainCreateInfoKHR(vk::SwapchainCreateFlagsKHR(), info.surface, info.image_count, info.format.format, info.format.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment);
    if (__detail::__vulkan_physical_device_info.graphics_queue_index != __detail::__vulkan_physical_device_info.present_queue_index) {
        ELM_DEBUG("Using concurrent sharing mode");
        std::uint32_t queues[] = {__detail::__vulkan_physical_device_info.graphics_queue_index, __detail::__vulkan_physical_device_info.present_queue_index};
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        create_info.pQueueFamilyIndices = queues;
        create_info.queueFamilyIndexCount = 2;
    } else {
        ELM_DEBUG("Using exclusive sharing mode");
        create_info.imageSharingMode = vk::SharingMode::eExclusive;
    }
    create_info.preTransform = info.pre_transform;
    create_info.compositeAlpha = info.composite_alpha;
    create_info.presentMode = info.present_mode;
    create_info.clipped = info.clip ? VK_TRUE : VK_FALSE;
    create_info.oldSwapchain = nullptr;
    vk::SwapchainKHR swapchain = __detail::__vulkan_device.createSwapchainKHR(create_info);
    ELM_INFO("Swapchain created");
    swapchain_info sinfo;
    sinfo.swapchain = swapchain;
    std::vector<vk::Image> images = __detail::__vulkan_device.getSwapchainImagesKHR(swapchain);
    sinfo.image_data.reserve(images.size());
    sinfo.width = info.width;
    sinfo.height = info.height;
    for (vk::Image& image : images) {
        vk::ImageViewCreateInfo img_view_info;
        img_view_info.image = image;
        img_view_info.viewType = vk::ImageViewType::e2D;
        img_view_info.format = info.format.format;
        img_view_info.components.r = vk::ComponentSwizzle::eIdentity;
        img_view_info.components.g = vk::ComponentSwizzle::eIdentity;
        img_view_info.components.b = vk::ComponentSwizzle::eIdentity;
        img_view_info.components.a = vk::ComponentSwizzle::eIdentity;
        img_view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        img_view_info.subresourceRange.baseMipLevel = 0;
        img_view_info.subresourceRange.levelCount = 1;
        img_view_info.subresourceRange.baseArrayLayer = 0;
        img_view_info.subresourceRange.layerCount = 1;
        swapchain_image_data img_data;
        img_data.image = image,
        img_data.image_view = __detail::__vulkan_device.createImageView(img_view_info);
        sinfo.image_data.push_back(img_data);
    }
    return sinfo;
}

void vulkan::destroy_swapchain(vulkan::swapchain_info& info) {
    ELM_INFO("Destroying swapchain");
    for (swapchain_image_data& img_data : info.image_data) {
        __detail::__vulkan_device.destroyImageView(img_data.image_view);
    }
    __detail::__vulkan_device.destroySwapchainKHR(info.swapchain);
}