#pragma once

#include <graphics/vulkan.h>
#include <graphics/vulkan_render_target.h>
#include <graphics/vulkan_buffer.h>
#include <graphics/vulkan_descriptor.h>
#include <render/scene_data_manager.h>
#include <scenegraph/nodes/camera_node_ref.h>
#include <scenegraph/transform.h>
#include <utils/uuid.h>
#include <glm/glm.hpp>

namespace element {
    namespace render {
        struct camera_data {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 view_projection;
        };

        class scene_renderer {
            private:
                static void create_forward_renderpass();

                std::uint32_t width;
                std::uint32_t height;

                scene_render_data* scene_data = nullptr;
                scenegraph::camera_node_ref camera;
                scenegraph::transform_watcher cam_watcher;
                camera_data cam_data;
                vulkan::upload_buffer cam_gpu_data;
                
                vulkan::descriptor_set global_descriptorset;
                vulkan::color_attachment color;
                vulkan::depth_attachment depth;
                vk::Framebuffer framebuffer;
                
                void create_framebuffer();
                void create_descriptorset();
            public:
                static vk::RenderPass forward_renderpass;

                static void init();
                static void cleanup();

                scene_renderer(std::uint32_t width, std::uint32_t height);
                ~scene_renderer();
                void resize(std::uint32_t width, std::uint32_t height);
                void record_render(vk::CommandBuffer& cmd);
                void record_sync_camera(vk::CommandBuffer& cmd);
                void select_scene(const uuid& id, const scenegraph::camera_node_ref& camera);
                void select_scene(const uuid& id);
                void select_camera(const scenegraph::camera_node_ref& camera);

                inline std::uint32_t get_width() const {return width;}
                inline std::uint32_t get_height() const {return height;}
                inline vk::Image get_color_image() const {return color.get_target();}
                inline vk::Image get_depth_image() const {return depth.get_target();}
        };

    } // namespace render
} // namespace element
