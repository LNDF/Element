#pragma once

#include <graphics/vulkan_buffer.h>
#include <glm/glm.hpp>
#include <vector>

namespace element {
    namespace render {
        struct vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 tex_coords;
        };

        struct mesh {
            std::vector<vertex> vertices;
            std::vector<std::uint32_t> indices;
        };

        class gpu_mesh {
            private:
                vulkan::device_buffer vertex_buffer;
                vulkan::device_buffer index_buffer;
            public:
                gpu_mesh(vulkan::device_buffer&& vertex_buffer, vulkan::device_buffer&& index_buffer);

                gpu_mesh(const gpu_mesh& other) = delete;
                gpu_mesh& operator=(const gpu_mesh& other) = delete;
                gpu_mesh(gpu_mesh&& other) = default;
                gpu_mesh& operator=(gpu_mesh&& other) = default;

                void record_bind_buffer(vk::CommandBuffer& cmd);
                void record_mesh_upload(vk::CommandBuffer& cmd);
                void delete_staging_buffers();
        };
    } // namespace render
} // namespace element
