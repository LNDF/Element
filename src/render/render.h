#pragma once

#include <render/vulkan_swapchain.h>
#include <render/vulkan.h>
#include <render/scene_render.h>

#define ELM_MAX_FRAMES_IN_FLIGHT 3

namespace element {
    namespace render {
        extern bool renderer_initialized;

        void select_swapchain(const vulkan::swapchain_info& info);
        void unselect_swapchain();

        void init_renderer();
        void cleanup_renderer();
        void reset_renderer();
        void reset_renderer_later();

        void render_screen();
        void render_screen_safe();

        scene_renderer* get_screen_scene_renderer();        
    } // namespace render
} // namespace element
