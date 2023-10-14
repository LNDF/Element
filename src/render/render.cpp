#include "render.h"

#include <core/log.h>
#include <graphics/vulkan_command_buffer.h>
#include <graphics/vulkan_sync.h>

using namespace element;

std::uint32_t render::frames_in_flight = ELM_MAX_FRAMES_IN_FLIGHT;
std::uint32_t render::current_frame = 0;
render::swapchain_frame_info render::swapchain_frames[ELM_MAX_FRAMES_IN_FLIGHT];
const vulkan::swapchain_info* render::current_swapchain = nullptr;
vk::CommandBuffer render::main_command_buffer;

void render::select_swapchain(const vulkan::swapchain_info& info) {
    current_swapchain = &info;
    if (info.image_data.size() < ELM_MAX_FRAMES_IN_FLIGHT) {
        frames_in_flight = info.image_data.size();
    }
    current_frame = 0;
    ELM_DEBUG("Using {} frames in flight", frames_in_flight);
}

void render::unselect_swapchain() {
    //TODO: wait until all gpu work is done
    current_swapchain = nullptr;
    frames_in_flight = ELM_MAX_FRAMES_IN_FLIGHT;
    current_frame = 0;
}

void render::init_renderer() {
    ELM_INFO("Initializing renderer...");
    main_command_buffer = vulkan::create_command_buffer();
    ELM_DEBUG("Creating sync structures...");
    for (std::uint32_t i = 0; i < ELM_MAX_FRAMES_IN_FLIGHT; ++i) {
        swapchain_frames[i].command_buffer = vulkan::create_command_buffer();
        swapchain_frames[i].fence = vulkan::create_fence();
        swapchain_frames[i].image_acquired = vulkan::create_semaphore();
        swapchain_frames[i].render_done = vulkan::create_semaphore();
    }
}

void render::cleanup_renderer() {
    ELM_INFO("Cleanning up renderer...");
    vulkan::device.waitIdle();
    for (std::uint32_t i = 0; i < ELM_MAX_FRAMES_IN_FLIGHT; ++i) {
        vulkan::device.destroyFence(swapchain_frames[i].fence);
        vulkan::device.destroySemaphore(swapchain_frames[i].image_acquired);
        vulkan::device.destroySemaphore(swapchain_frames[i].render_done);
    }
}

void render::render() { //TODO: handle errors of waitForFences, resetFences, presentKHR
    swapchain_frame_info& frame = swapchain_frames[current_frame];
    vulkan::device.waitForFences(1, &frame.fence, VK_TRUE, UINT64_MAX);
    vulkan::device.resetFences(1, &frame.fence);
    std::uint32_t image_index = vulkan::device.acquireNextImageKHR(current_swapchain->swapchain, UINT64_MAX, frame.image_acquired, nullptr).value;
    frame.command_buffer.reset();

    //TODO: record command buffer

    vk::PipelineStageFlags wait_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submit_info;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &frame.image_acquired;
    submit_info.pWaitDstStageMask = &wait_mask;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &frame.command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &frame.render_done;
    vulkan::graphics_queue.submit(submit_info, frame.fence);
    vk::PresentInfoKHR present_info;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &frame.render_done;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &current_swapchain->swapchain;
    present_info.pImageIndices = &image_index;
    vulkan::present_queue.presentKHR(present_info);
    current_frame = (current_frame + 1) % frames_in_flight;
}