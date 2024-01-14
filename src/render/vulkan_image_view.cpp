#include "vulkan_image_view.h"

using namespace element;

vk::ImageView vulkan::create_image_view(vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspect, std::uint32_t levels, std::uint32_t layers) {
    vk::ImageViewCreateInfo info;
    info.flags = vk::ImageViewCreateFlags();
    info.image = image;
    info.format = format;
    info.viewType = type;
    info.components.r = vk::ComponentSwizzle::eIdentity;
    info.components.g = vk::ComponentSwizzle::eIdentity;
    info.components.b = vk::ComponentSwizzle::eIdentity;
    info.components.a = vk::ComponentSwizzle::eIdentity;
    info.subresourceRange.aspectMask = aspect;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = levels;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = layers;
    return device.createImageView(info);
}