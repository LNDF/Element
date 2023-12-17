#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace render {
        class scene_renderer {
            private:
                static void create_forward_renderpass();
            public:
                static vk::RenderPass forward_renderpass;

                static void init();
                static void cleanup();
        };

    } // namespace render
} // namespace element
