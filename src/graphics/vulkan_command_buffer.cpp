#include "vulkan_command_buffer.h"

using namespace element;

vk::CommandBuffer vulkan::create_command_buffer() {
    vk::CommandBufferAllocateInfo info;
    info.commandPool = command_pool;
    info.level = vk::CommandBufferLevel::ePrimary;
    info.commandBufferCount = 1;
    return device.allocateCommandBuffers(info)[0];
}