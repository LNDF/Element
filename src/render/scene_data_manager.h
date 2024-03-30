#pragma once

#include <render/vulkan_buffer.h>
#include <scenegraph/transform.h>
#include <utils/packed_map.h>
#include <utils/uuid.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include <unordered_map>

namespace element {
    namespace scenegraph {
        class mesh_node;
    }

    namespace render {
        
        class render_graph_mesh_instance_data {
            private:
                std::vector<glm::mat4> model_matrices;
                vulkan::upload_buffer model_matrices_gpu;
                bool recreate = false;
                packed_map<uuid, std::pair<std::uint32_t, scenegraph::transform_watcher>> instances_watchers;
            public:
                render_graph_mesh_instance_data();
                render_graph_mesh_instance_data(render_graph_mesh_instance_data&& other) = default;
                render_graph_mesh_instance_data& operator=(render_graph_mesh_instance_data&& other) = default;
                render_graph_mesh_instance_data(const render_graph_mesh_instance_data& other) = delete;
                render_graph_mesh_instance_data& operator=(const render_graph_mesh_instance_data& other) = delete;

                inline std::uint32_t get_instance_count() const {return instances_watchers.size();}
                inline vk::Buffer get_model_matrix_buffer() const {return model_matrices_gpu.get_buffer();}

                void add_instance(const scenegraph::mesh_node& instance);
                void remove_instance(const uuid& id);

                void sync(vk::CommandBuffer& cmd);
        };

        /** 
         *  Scene render graph:
         *  - pipeline (id):
         *    - material (id):
         *      - mesh (id): render graph mesh data
         * CHANGE TO PACKED MAP ASAP
        */ 
        using render_graph = std::unordered_map<uuid, std::unordered_map<uuid, std::unordered_map<uuid, render_graph_mesh_instance_data>>>;

        struct render_graph_mesh_data {
            uuid material;
            uuid mesh;
            bool enabled;
        };

        class scene_render_data {
            private:
                std::unordered_map<uuid, render_graph_mesh_data> mesh_data;
                render_graph graph;
            public:
                scene_render_data();
                ~scene_render_data();

                void register_node(const scenegraph::mesh_node& node);
                void unregister_node(const uuid& id);
                void enable_node(const scenegraph::mesh_node& node);
                void disable_node(const uuid& id);
                void enable_material(const uuid& material, const uuid& pipeline);
                void disable_material(const uuid& material, const uuid& pipeline);
                void update_material(const uuid& material, const uuid& old_pipeline, const uuid& new_pipeline);
                void gpu_sync(vk::CommandBuffer& cmd);

                inline const render_graph& get_render_graph() const {return graph;}
        };

        void create_scene_render_data(const uuid& id);
        scene_render_data* get_scene_render_data(const uuid& id);
        void destroy_scene_render_data(const uuid& id);
        void destroy_all_scene_render_data();
        void record_sync_scene_render_data(vk::CommandBuffer& cmd);
        std::unordered_map<element::uuid, element::render::scene_render_data>& get_scene_render_data_map();

    } // namespace render
} // namespace element
