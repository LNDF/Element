#pragma once

#include <render/vulkan.h>

namespace element {
    namespace vulkan {
        
        vk::ImageView create_image_view(vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspect, std::uint32_t levels, std::uint32_t layers);

    } // namespace vulkan
} // namespace element
