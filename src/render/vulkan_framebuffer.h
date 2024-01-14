#pragma once

#include <render/vulkan.h>

namespace element {
    namespace vulkan {
        
        vk::Framebuffer create_framebuffer(vk::RenderPass renderpass, vk::ImageView* attachments, std::uint32_t attachment_count, std::uint32_t width, std::uint32_t height);

    } // namespace vulkan
} // namespace element 
