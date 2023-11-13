#pragma once

#include <graphics/vulkan.h>
#include <graphics/vulkan_swapchain.h>
#include <glm/glm.hpp>

namespace element {
    namespace render {
        
        struct scene_global_data {
            glm::vec2 mouse_position;
            glm::mat4 view_matrix;
        };

        class scene_renderer {
            private:
                static void create_forward_renderpass();
                static void create_global_descriptorset_layout();
            public:
                static vk::RenderPass forward_renderpass;
                static vk::DescriptorSetLayout global_descriptorset_layout;
                static bool initialized;

                static void init();
                static void cleanup();
        };

    } // namespace render
} // namespace element
