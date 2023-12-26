#include "vulkan_buffer.h"

#include <cstring>

using namespace element;

vulkan::upload_buffer::upload_buffer(vk::BufferUsageFlags usage) : upload_buffer(0, usage) {}

vulkan::upload_buffer::upload_buffer(std::uint32_t size, vk::BufferUsageFlags usage) : usage(usage), size(size), capacity(size) {
    reset_buffers();
}

vulkan::upload_buffer::~upload_buffer() {
    destroy_buffers();
}

vulkan::upload_buffer::upload_buffer(upload_buffer&& other)
    : buffer_alloc(std::move(other.buffer_alloc)), staging_alloc(std::move(other.staging_alloc)), usage(std::move(other.usage)),
      buffer(std::move(other.buffer)), staging(std::move(other.staging)),
      size(std::move(other.size)), capacity(std::move(other.capacity)), upload_pending(std::move(other.upload_pending)) {
    other.buffer_alloc = nullptr;
    other.staging_alloc = nullptr;
}

vulkan::upload_buffer& vulkan::upload_buffer::operator=(upload_buffer&& other) {
    destroy_buffers();
    buffer_alloc = std::move(other.buffer_alloc);
    staging_alloc = std::move(other.staging_alloc);
    usage = std::move(other.usage);
    buffer = std::move(other.buffer);
    staging = std::move(other.staging);
    size = std::move(other.size);
    capacity = std::move(other.capacity);
    upload_pending = std::move(other.upload_pending);
    other.buffer_alloc = nullptr;
    other.staging_alloc = nullptr;
    return *this;
}

void vulkan::upload_buffer::create_staging() {
    vk::BufferCreateInfo staging_info;
    staging_info.size = capacity;
    staging_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
    VmaAllocationCreateInfo staging_alloc_create_info = {};
    staging_alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    staging_alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VmaAllocationInfo staging_alloc_info;
    vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&staging_info), &staging_alloc_create_info, reinterpret_cast<VkBuffer*>(&staging), &staging_alloc, &staging_alloc_info);
}

void vulkan::upload_buffer::reset_buffers() {
    if (capacity == 0) return;
    destroy_buffers();
    vk::BufferCreateInfo buffer_info;
    buffer_info.size = capacity;
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
    }
}

void vulkan::upload_buffer::destroy_buffers() {
    if (staging_alloc != nullptr) {
        vmaDestroyBuffer(allocator, staging, staging_alloc);
        staging_alloc = nullptr;
    }
    if (buffer_alloc != nullptr) {
        vmaDestroyBuffer(allocator, buffer, buffer_alloc);
        buffer_alloc = nullptr;
    }
}

void vulkan::upload_buffer::reserve(std::uint32_t capacity) {
    this->capacity = capacity;
    reset_buffers();
}

void vulkan::upload_buffer::resize(std::uint32_t size) {
    this->size = size;
    if (this->size > this->capacity) {
        reserve(size);
    }
}

void vulkan::upload_buffer::set(const void* data) {
    if (capacity == 0) return;
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

void vulkan::upload_buffer::record_upload(vk::CommandBuffer& cmd) {
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

void vulkan::upload_buffer::destroy_staging() {
    if (staging != nullptr) {
        vmaDestroyBuffer(allocator, staging, staging_alloc);
        staging = nullptr;
    }
}