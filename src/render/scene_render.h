#pragma once

#include <graphics/vulkan.h>
#include <graphics/vulkan_swapchain.h>

namespace element {
    namespace render {
        
        class scene_renderer {
            private:
                static void create_forward_renderpass();
            public:
                static vk::RenderPass forward_renderpass;
                static bool initialized;

                static void init();
                static void cleanup();
        };

    } // namespace render
} // namespace element
