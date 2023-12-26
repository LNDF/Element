#pragma once

#include <graphics/vulkan_swapchain.h>
#include <graphics/vulkan.h>

#define ELM_MAX_FRAMES_IN_FLIGHT 3

namespace element {
    namespace render {
        extern bool renderer_initialized;

        void select_swapchain(const vulkan::swapchain_info& info);
        void unselect_swapchain();

        void init_renderer();
        void cleanup_renderer();

        void render_screen();
    } // namespace render
} // namespace element
