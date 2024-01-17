#include "scene_data_manager.h"

#include <scenegraph/nodes/mesh_node.h>
#include <scenegraph/nodes/mesh_node_ref.h>
#include <scenegraph/scene.h>
#include <render/mesh_manager.h>
#include <render/material_manager.h>

using namespace element;

static std::unordered_map<uuid, render::scene_render_data> render_data_scenes;

render::render_graph_mesh_instance_data::render_graph_mesh_instance_data() : model_matrices_gpu(vk::BufferUsageFlagBits::eVertexBuffer) {}

void render::render_graph_mesh_instance_data::add_instance(const scenegraph::mesh_node& instance) {
    recreate = true;
    scenegraph::transform_watcher watcher(instance.get_transform());
    instances_watchers.try_emplace(instance.get_id(), 0, watcher);
}

void render::render_graph_mesh_instance_data::remove_instance(const uuid& id) {
    auto it = instances_watchers.find(id);
    if (it == instances_watchers.end()) return;
    recreate = true;
    instances_watchers.erase(it);
}

void render::render_graph_mesh_instance_data::sync(vk::CommandBuffer& cmd) {
    if (recreate) {
        recreate = false;
        std::uint32_t i = 0;
        model_matrices.clear();
        model_matrices.reserve(instances_watchers.size());
        model_matrices_gpu.reserve(sizeof(glm::mat4) * model_matrices.capacity());
        model_matrices_gpu.resize(sizeof(glm::mat4) * instances_watchers.size());
        for (auto& [id, instance] : instances_watchers) {
            instance.first = i++;
            instance.second.has_updated(); //reset
            model_matrices.push_back(instance.second.get_transform().get_world_matrix());
        }
        model_matrices_gpu.set(model_matrices.data());
    } else {
        bool upload = false;
        for (auto& [id, instance] : instances_watchers) {
            if (instance.second.has_updated()) {
                upload = true;
                model_matrices[instance.first] = instance.second.get_transform().get_world_matrix();
            }
        }
        if (upload) model_matrices_gpu.set(model_matrices.data());
    }
    model_matrices_gpu.record_upload(cmd);
}

render::scene_render_data::scene_render_data() {

}

render::scene_render_data::~scene_render_data() {

}

void render::scene_render_data::register_node(const scenegraph::mesh_node& node) {
    render_graph_mesh_data data;
    data.material = node.get_material();
    data.mesh = node.get_mesh();
    data.enabled = false;
    auto it = mesh_data.find(node.get_id());
    bool was_enabled = false;
    if (it != mesh_data.end() && it->second.enabled) {
        was_enabled = true;
    }
    if (was_enabled) {
        disable_node(node.get_id());
        unregister_node(node.get_id());
    }
    mesh_data.insert_or_assign(node.get_id(), data);
    gpu_mesh_manager::claim_resource(data.mesh);
    if (was_enabled) enable_node(node);
}

void render::scene_render_data::unregister_node(const uuid& id) {
    auto it = mesh_data.find(id);
    if (it != mesh_data.end()) {
        if (it->second.enabled) disable_node(id);
        gpu_mesh_manager::release_resource(it->second.mesh);
        mesh_data.erase(it);
    }
}

void render::scene_render_data::enable_node(const scenegraph::mesh_node& node) {
    auto it = mesh_data.find(node.get_id());
    if (it != mesh_data.end()) {
        if (it->second.enabled) return;
        it->second.enabled = true;
        const material* material = material_manager::get(it->second.material);
        if (material == nullptr) return;
        const uuid& pipeline = material->get_pipeline_id();
        render_graph_mesh_instance_data& instance_data = graph[pipeline][it->second.material][it->second.mesh];
        instance_data.add_instance(node);
    }
}

void render::scene_render_data::disable_node(const uuid& id) {
    auto it = mesh_data.find(id);
    if (it != mesh_data.end()) {
        if (!it->second.enabled) return;
        it->second.enabled = false;
        const material* material = material_manager::get(it->second.material);
        if (material == nullptr) return;
        const uuid& pipeline = material->get_pipeline_id();
        auto materials_it = graph.find(pipeline);
        auto& materials = materials_it->second;
        auto meshes_it = materials.find(it->second.material);
        auto& meshes = meshes_it->second;
        auto instance_data_it = meshes.find(it->second.mesh);
        render_graph_mesh_instance_data& instance_data = instance_data_it->second;
        instance_data.remove_instance(id);
        if (instance_data.get_instance_count() == 0) meshes.erase(instance_data_it);
        if (meshes.empty()) materials.erase(meshes_it);
        if (materials.empty()) graph.erase(materials_it);
    }
}

void render::scene_render_data::enable_material(const uuid& material, const uuid& pipeline) {
    for (const auto& [id, data] : mesh_data) {
        if (data.enabled && data.material == material) {
            render_graph_mesh_instance_data& instance_data = graph[pipeline][data.material][data.mesh];
            scenegraph::mesh_node_ref ref = id;
            instance_data.add_instance(*ref.get_node());
        }
    }
}

void render::scene_render_data::disable_material(const uuid& material, const uuid& pipeline) {
    auto it = graph.find(pipeline);
    if (it != graph.end()) {
        it->second.erase(material);
        if (it->second.empty()) graph.erase(it);
    }
}

void render::scene_render_data::update_material(const uuid& material, const uuid& old_pipeline, const uuid& new_pipeline) {
    auto it = graph.find(old_pipeline);
    if (it == graph.end()) return;
    auto it2 = it->second.find(material);
    if (it2 != it->second.end()) {
        graph[new_pipeline][material] = std::move(it2->second);
    }
}

void render::scene_render_data::gpu_sync(vk::CommandBuffer& cmd) {
    for (auto& pipeline : graph) {
        for (auto& material : pipeline.second) {
            for (auto& mesh : material.second) {
                mesh.second.sync(cmd);
            }
        }
    }
}

void render::create_scene_render_data(const uuid& id) {
    render_data_scenes[id];
}


render::scene_render_data* render::get_scene_render_data(const uuid& id) {
    auto it = render_data_scenes.find(id);
    if (it == render_data_scenes.end()) return nullptr;
    return &it->second;
}

void render::destroy_scene_render_data(const uuid& id) {
    render_data_scenes.erase(id);
}

void render::destroy_all_scene_render_data() {
    render_data_scenes.clear();
}

void render::record_sync_scene_render_data(vk::CommandBuffer& cmd) {
    for (auto& [id, scene_data] : render_data_scenes) {
        scene_data.gpu_sync(cmd);
    }
}

std::unordered_map<element::uuid, element::render::scene_render_data>& render::get_scene_render_data_map() {
    return render_data_scenes;
}