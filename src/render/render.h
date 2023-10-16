#pragma once

#include <graphics/vulkan_swapchain.h>
#include <graphics/vulkan.h>

#define ELM_MAX_FRAMES_IN_FLIGHT 3

namespace element {
    namespace render {
        struct swapchain_frame_info {
            vk::CommandBuffer command_buffer;
            vk::Semaphore image_acquired, render_done;
            vk::Fence fence;
        };

        extern bool renderer_initialized;

        extern std::uint32_t frames_in_flight;
        extern std::uint32_t current_frame;
        extern swapchain_frame_info swapchain_frames[ELM_MAX_FRAMES_IN_FLIGHT];
        extern const vulkan::swapchain_info* current_swapchain;
        extern vk::CommandBuffer main_command_buffer;

        void select_swapchain(const vulkan::swapchain_info& info);
        void unselect_swapchain();

        void init_renderer();
        void cleanup_renderer();

        void render();
    } // namespace render
} // namespace element
