#include "render.h"

#include <stdexcept>
#include <core/log.h>
#include <graphics/vulkan_command_buffer.h>
#include <graphics/vulkan_sync.h>
#include <graphics/vulkan_image.h>
#include <graphics/vulkan_render_target.h>
#include <render/global_data.h>
#include <render/pipeline.h>
#include <render/material.h>
#include <render/mesh_manager.h>
#include <render/render_sync.h>
#include <render/scene_render.h>

using namespace element;

bool render::renderer_initialized = false;

static const vulkan::swapchain_info* current_swapchain = nullptr;
static render::scene_renderer* screen_scene_renderer = nullptr;
static vk::Semaphore image_acquired, render_done;
static vk::Fence render_submitted;
static vk::CommandBuffer render_command_buffer;

static vk::Semaphore sync_done;
static vk::Fence sync_submitted;
static vk::CommandBuffer sync_command_buffer;

static void sync_resources() {
    if (vulkan::device.waitForFences(sync_submitted, VK_TRUE, UINT64_MAX) == vk::Result::eTimeout) {
        throw std::runtime_error("Timeout when waiting for fence.");
    }
    vulkan::device.resetFences(sync_submitted);
    sync_command_buffer.reset();
    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    sync_command_buffer.begin(begin_info);
    render::record_renderer_sync(sync_command_buffer);
    screen_scene_renderer->record_sync_camera(sync_command_buffer);
    sync_command_buffer.end();
    vk::SubmitInfo submit_info;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &sync_command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &sync_done;
    vulkan::graphics_queue.submit(submit_info, sync_submitted);
}

static void render_present() { //Handle VK_ERROR_OUT_OF_DATE_KHR on acquireNextImageKHR
    std::uint32_t image_index;
    try {
        image_index = vulkan::device.acquireNextImageKHR(current_swapchain->swapchain, UINT64_MAX, image_acquired, nullptr).value;
    } catch (vk::OutOfDateKHRError& err) {
        ELM_DEBUG("Skipping frame");
        return;
    }
    if (vulkan::device.waitForFences(render_submitted, VK_TRUE, UINT64_MAX) == vk::Result::eTimeout) {
        throw std::runtime_error("Timeout when waiting for fence.");
    }
    vulkan::device.resetFences(render_submitted);
    render_command_buffer.reset();
    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    render_command_buffer.begin(begin_info);
    screen_scene_renderer->record_render(render_command_buffer);
    vulkan::transition_image_layout(render_command_buffer, current_swapchain->images[image_index], current_swapchain->format.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor, 1, 1);
    vulkan::transition_image_layout(render_command_buffer, screen_scene_renderer->get_color_image(), vulkan::color_attachment::format, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal, vulkan::color_attachment::aspect, 1, 1);
    vk::ImageBlit image_blit;
    image_blit.srcSubresource.aspectMask = vulkan::color_attachment::aspect;
    image_blit.srcSubresource.mipLevel = 0;
    image_blit.srcSubresource.baseArrayLayer = 0;
    image_blit.srcSubresource.layerCount = 1;
    image_blit.srcOffsets[0].x = 0;
    image_blit.srcOffsets[0].y = 0;
    image_blit.srcOffsets[0].z = 0;
    image_blit.srcOffsets[1].x = screen_scene_renderer->get_width();
    image_blit.srcOffsets[1].y = screen_scene_renderer->get_height();
    image_blit.srcOffsets[1].z = 1;
    image_blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    image_blit.dstSubresource.mipLevel = 0;
    image_blit.dstSubresource.baseArrayLayer = 0;
    image_blit.dstSubresource.layerCount = 1;
    image_blit.dstOffsets[0].x = 0;
    image_blit.dstOffsets[0].y = 0;
    image_blit.dstOffsets[0].z = 0;
    image_blit.dstOffsets[1].x = current_swapchain->width;
    image_blit.dstOffsets[1].y = current_swapchain->height;
    image_blit.dstOffsets[1].z = 1;
    render_command_buffer.blitImage(screen_scene_renderer->get_color_image(), vk::ImageLayout::eTransferSrcOptimal, current_swapchain->images[image_index], vk::ImageLayout::eTransferDstOptimal, image_blit, vk::Filter::eLinear);
    vulkan::transition_image_layout(render_command_buffer, current_swapchain->images[image_index], current_swapchain->format.format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor, 1, 1);
    render_command_buffer.end();
    vk::PipelineStageFlags wait_mask[2] = {vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer};
    vk::Semaphore submit_wait_semaphores[2] = {sync_done, image_acquired};
    vk::SubmitInfo submit_info;
    submit_info.waitSemaphoreCount = 2;
    submit_info.pWaitSemaphores = submit_wait_semaphores;
    submit_info.pWaitDstStageMask = wait_mask;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_done;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &render_command_buffer;
    vulkan::graphics_queue.submit(submit_info, render_submitted);
    vk::PresentInfoKHR present_info;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_done;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &current_swapchain->swapchain;
    present_info.pImageIndices = &image_index;
    vk::Result present_result;
    try {
        present_result = vulkan::present_queue.presentKHR(present_info);
    } catch (vk::OutOfDateKHRError& error) {
        present_result = vk::Result::eErrorOutOfDateKHR;
    }
    if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR) {
        ELM_DEBUG("Subotimal or outdated frame");
    }
}

void render::select_swapchain(const vulkan::swapchain_info& info) {
    vulkan::device.waitIdle();
    current_swapchain = &info;
    if (screen_scene_renderer == nullptr) {
        screen_scene_renderer = new scene_renderer(info.width, info.height);
    } else {
        screen_scene_renderer->resize(info.width, info.height);
    }
}

void render::unselect_swapchain() {
    vulkan::device.waitIdle();
    current_swapchain = nullptr;
}

void render::init_renderer() {
    if (renderer_initialized) return;
    ELM_INFO("Initializing renderer...");
    render_command_buffer = vulkan::create_command_buffer();
    sync_command_buffer = vulkan::create_command_buffer();
    ELM_DEBUG("Creating sync structures...");
    image_acquired = vulkan::create_semaphore();
    render_done = vulkan::create_semaphore();
    render_submitted = vulkan::create_fence();
    sync_done = vulkan::create_semaphore();
    sync_submitted = vulkan::create_fence();
    global_data::init();
    scene_renderer::init();
    renderer_initialized = true;
}

void render::cleanup_renderer() {
    if (!renderer_initialized) return;
    ELM_INFO("Cleanning up renderer...");
    vulkan::device.waitIdle();
    delete screen_scene_renderer;
    screen_scene_renderer = nullptr;
    scene_renderer::cleanup();
    destroy_all_materials();
    destroy_all_pipelines();
    gpu_mesh_manager::destroy_all_resources();
    global_data::cleanup();
    vulkan::device.destroySemaphore(image_acquired);
    vulkan::device.destroySemaphore(render_done);
    vulkan::device.destroyFence(render_submitted);
    vulkan::device.destroySemaphore(sync_done);
    vulkan::device.destroyFence(sync_submitted);
    vulkan::free_command_buffer(sync_command_buffer);
    vulkan::free_command_buffer(render_command_buffer);
    renderer_initialized = false;
}

void render::reset_renderer() {
    if (!renderer_initialized) return;
    vulkan::device.waitIdle();
    sync_command_buffer.reset();
    render_command_buffer.reset();
}

void render::render_screen() {
    sync_resources();
    render_present();
}

void render::render_screen_safe() {
    if (screen_scene_renderer != nullptr) render_screen();
}

render::scene_renderer* render::get_screen_scene_renderer() {
    return screen_scene_renderer;
}