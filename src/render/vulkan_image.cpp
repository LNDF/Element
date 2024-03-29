#include "vulkan_image.h"

#include <event/event.h>
#include <render/render_events.h>
#include <render/render.h>
#include <vector>
#include <utility>

using namespace element;

static std::vector<std::pair<VmaAllocation, vk::Image>> queued_deletions;

static std::pair<vk::AccessFlags, vk::PipelineStageFlags> get_access_and_stage(vk::ImageLayout layout, bool is_dest) {
    switch (layout) {
        case vk::ImageLayout::eUndefined:
            return std::make_pair(vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eTopOfPipe);
        case vk::ImageLayout::eGeneral:
            return std::make_pair(vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eHost);
        case vk::ImageLayout::ePreinitialized:
            return std::make_pair(vk::AccessFlagBits::eHostWrite, vk::PipelineStageFlagBits::eHost);
        case vk::ImageLayout::eTransferSrcOptimal:
            return std::make_pair(vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer);
        case vk::ImageLayout::eTransferDstOptimal:
            return std::make_pair(vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer);
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            return std::make_pair(vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eFragmentShader);
        case vk::ImageLayout::eColorAttachmentOptimal:
            return std::make_pair(vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput);
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            return std::make_pair(vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::PipelineStageFlagBits::eEarlyFragmentTests);
        case vk::ImageLayout::ePresentSrcKHR:
            return std::make_pair(vk::AccessFlagBits::eNone, vk::PipelineStageFlagBits::eBottomOfPipe);
        default:
            return std::make_pair(vk::AccessFlagBits::eNone, is_dest ? vk::PipelineStageFlagBits::eTopOfPipe : vk::PipelineStageFlagBits::eBottomOfPipe);
    }
}

vulkan::image::image(VmaAllocationCreateFlags alloc_flags) : alloc_flags(alloc_flags) {}

vulkan::image::image(const vk::ImageCreateInfo& info, VmaAllocationCreateFlags alloc_flags) : alloc_flags(alloc_flags) {
    create_image(info);
}

vulkan::image::~image() {
    destroy_image();
}

void vulkan::image::destroy_image() {
    if (image_alloc != nullptr) {
        queued_deletions.emplace_back(image_alloc, vk_image);
        render::reset_renderer_later();
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

void vulkan::transition_image_layout(vk::CommandBuffer& cmd, vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::ImageAspectFlags aspect_mask, std::uint32_t levels, std::uint32_t layers) {
    vk::ImageMemoryBarrier barrier;
    auto [src_access_mask, src_stage] = get_access_and_stage(old_layout, false);
    auto [dst_access_mask, dst_stage] = get_access_and_stage(new_layout, true);
    barrier.image = image;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.subresourceRange.aspectMask = aspect_mask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layers;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstAccessMask = dst_access_mask;
    cmd.pipelineBarrier(src_stage, dst_stage, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &barrier);
}

static bool image_delete(events::render_idle& event) {
    for (auto& [allocation, image] : queued_deletions) {
        vmaDestroyImage(vulkan::allocator, image, allocation);
    }
    queued_deletions.clear();
    return true;
}

ELM_REGISTER_EVENT_CALLBACK(events::render_idle, image_delete, event_callback_priority::highest)