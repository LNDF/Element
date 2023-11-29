#include "vulkan_buffer.h"

#include <cstring>

using namespace element;

vulkan::device_buffer::device_buffer(std::uint32_t size, vk::BufferUsageFlags usage) : size(size) {
    vk::BufferCreateInfo buffer_info;
    buffer_info.size = size;
    buffer_info.usage = vk::BufferUsageFlagBits::eTransferDst | usage;
    VmaAllocationCreateInfo buffer_alloc_create_info = {};
    buffer_alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    buffer_alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                                   | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
    VmaAllocationInfo buffer_alloc_info;
    vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&buffer_info), &buffer_alloc_create_info, reinterpret_cast<VkBuffer*>(&buffer), &buffer_alloc, &buffer_alloc_info);
    VkMemoryPropertyFlags mem_flags;
    vmaGetAllocationMemoryProperties(allocator, buffer_alloc, &mem_flags);
    if (!(mem_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        staging_required = true;
        create_staging();
    }
}

vulkan::device_buffer::~device_buffer() {
    if (staging_alloc != nullptr) {
        vmaDestroyBuffer(allocator, staging, staging_alloc);
    }
    if (buffer_alloc != nullptr) {
        vmaDestroyBuffer(allocator, buffer, buffer_alloc);
    }
}

vulkan::device_buffer::device_buffer(device_buffer&& other)
    : buffer_alloc(std::move(other.buffer_alloc)), staging_alloc(std::move(staging_alloc)),
      buffer(std::move(other.buffer)), staging(std::move(other.staging)),
      size(std::move(other.size)), upload_pending(std::move(other.upload_pending)) {
    other.buffer_alloc = nullptr;
    other.staging_alloc = nullptr;
}

vulkan::device_buffer& vulkan::device_buffer::operator=(device_buffer&& other) {
    if (staging_alloc != nullptr) {
        vmaDestroyBuffer(allocator, staging, staging_alloc);
    }
    if (buffer_alloc != nullptr) {
        vmaDestroyBuffer(allocator, buffer, buffer_alloc);
    }
    buffer_alloc = std::move(other.buffer_alloc);
    staging_alloc = std::move(other.staging_alloc);
    buffer = std::move(other.buffer);
    staging = std::move(other.staging);
    size = std::move(other.size);
    upload_pending = std::move(other.upload_pending);
    other.buffer_alloc = nullptr;
    other.staging_alloc = nullptr;
    return *this;
}

void vulkan::device_buffer::create_staging() {
    vk::BufferCreateInfo staging_info;
    staging_info.size = size;
    staging_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
    VmaAllocationCreateInfo staging_alloc_create_info = {};
    staging_alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    staging_alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VmaAllocationInfo staging_alloc_info;
    vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&staging_info), &staging_alloc_create_info, reinterpret_cast<VkBuffer*>(&staging), &staging_alloc, &staging_alloc_info);
}

void vulkan::device_buffer::set(const void* data) {
    void* mapped;
    if (!staging_required) {
        vmaMapMemory(allocator, buffer_alloc, &mapped);
        memcpy(mapped, data, size);
        vmaUnmapMemory(allocator, buffer_alloc);
    } else {
        if (staging == nullptr) create_staging();
        vmaMapMemory(allocator, staging_alloc, &mapped);
        memcpy(mapped, data, size);
        vmaFlushAllocation(allocator, staging_alloc, 0, VK_WHOLE_SIZE);
        vmaUnmapMemory(allocator, staging_alloc);
        upload_pending = true;
    }
}

void vulkan::device_buffer::record_upload(vk::CommandBuffer& cmd) {
    if (!upload_pending) return;
    //Submission guarantees the host write being complete, as per
    //See: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#synchronization-submission-host-writes
    //So no need for a barrier before the transfer
    std::array<vk::BufferCopy, 1> regions;
    regions[0].srcOffset = 0;
    regions[0].dstOffset = 0;
    regions[0].size = size;
    cmd.copyBuffer(staging, buffer, regions);
    upload_pending = false;
}

void vulkan::device_buffer::delete_staging() {
    if (staging != nullptr) {
        vmaDestroyBuffer(allocator, staging, staging_alloc);
        staging = nullptr;
    }
}