#include "vulkan_image.h"

using namespace element;

vulkan::image::image(VmaAllocationCreateFlags alloc_flags) : alloc_flags(alloc_flags) {}

vulkan::image::image(const vk::ImageCreateInfo& info, VmaAllocationCreateFlags alloc_flags) : alloc_flags(alloc_flags) {
    create_image(info);
}

vulkan::image::~image() {
    destroy_image();
}

void vulkan::image::destroy_image() {
    if (image_alloc != nullptr) {
        vmaDestroyImage(allocator, vk_image, image_alloc);
        image_alloc = nullptr;
    }
}

void vulkan::image::create_image(const vk::ImageCreateInfo& info) {
    destroy_image();
    VmaAllocationCreateInfo alloc_create_info = {};
    VmaAllocationInfo alloc_info;
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = alloc_flags;
    alloc_create_info.priority = 1.0f;
    vmaCreateImage(allocator, reinterpret_cast<const VkImageCreateInfo*>(&info), &alloc_create_info, reinterpret_cast<VkImage*>(&vk_image), &image_alloc, &alloc_info);
}