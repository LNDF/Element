#include "vulkan_sync.h"

using namespace element;

vk::Semaphore vulkan::create_semaphore() {
    vk::SemaphoreCreateInfo info;
    info.flags = vk::SemaphoreCreateFlags();
    return device.createSemaphore(info);
}

vk::Fence vulkan::create_fence() {
    vk::FenceCreateInfo info;
    info.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;
    return device.createFence(info);
}