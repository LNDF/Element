#include "vulkan_framebuffer.h"

using namespace element;

vk::Framebuffer vulkan::create_framebuffer(vk::RenderPass renderpass, vk::ImageView* attachments, std::uint32_t attachment_count, std::uint32_t width, std::uint32_t height) {
    vk::FramebufferCreateInfo info;
    info.flags = vk::FramebufferCreateFlags();
    info.renderPass = renderpass;
    info.attachmentCount = attachment_count;
    info.pAttachments = attachments;
    info.width = width;
    info.height = height;
    info.layers = 1;
    return get_device().createFramebuffer(info);
}