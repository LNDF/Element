#pragma once

#include <graphics/vulkan.h>
#include <graphics/vulkan_render_target.h>
#include <scenegraph/nodes/camera_node_ref.h>
#include <utils/uuid.h>

namespace element {
    namespace render {
        class scene_renderer {
            private:
                static void create_forward_renderpass();

                std::uint32_t width;
                std::uint32_t height;
                
                vulkan::color_attachment color;
                vulkan::depth_attachment depth;
                vk::Framebuffer framebuffer;
                

                void destroy_render_target();
                void recreate_render_target();
            public:
                static vk::RenderPass forward_renderpass;

                static void init();
                static void cleanup();

                scene_renderer(std::uint32_t width, std::uint32_t height);
                void resize(std::uint32_t width, std::uint32_t height);
                void record_render(vk::CommandBuffer& cmd);
                void record_sync_camera(vk::CommandBuffer& cmd);
                void select_scene(const uuid& id, const scenegraph::camera_node_ref& camera);
                void select_scene(const uuid& id);
                void select_camera(const scenegraph::camera_node_ref& camera);
        };

    } // namespace render
} // namespace element
