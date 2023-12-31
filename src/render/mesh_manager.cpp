#include "mesh_manager.h"

#include <graphics/vulkan_buffer.h>
#include <utils/packed_map.h>
#include <unordered_map>
#include <vector>
#include <thread>

static std::unordered_map<element::uuid, element::render::gpu_mesh> gpu_meshes;
static element::packed_map<element::uuid, element::render::gpu_mesh*> upload_pending_gpu_meshes;
static element::packed_map<element::uuid, element::render::gpu_mesh*> staging_delete_pending_gpu_meshes;
static std::mutex sync_mutex;

using namespace element;

void __detail::__render_gpu_mesh_resource_loaded(const uuid& id, const render::mesh& mesh) {
    std::lock_guard<std::mutex> lk(sync_mutex);
    if (render::gpu_mesh_manager::get_resource_references(id) == 0) return;
    vulkan::upload_buffer vertex_buffer(mesh.vertices.size() * sizeof(render::vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vulkan::upload_buffer index_buffer(mesh.indices.size() * sizeof(std::uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
    vertex_buffer.set(mesh.vertices.data());
    index_buffer.set(mesh.indices.data());
    render::gpu_mesh new_mesh(std::move(vertex_buffer), std::move(index_buffer));
    render::gpu_mesh& inserted = gpu_meshes.insert_or_assign(id, std::move(new_mesh)).first->second;
    upload_pending_gpu_meshes[id] = &inserted;
}

void __detail::__render_gpu_mesh_destroy(const uuid& id) {
    std::lock_guard<std::mutex> lk(sync_mutex);
    upload_pending_gpu_meshes.erase(id);
    staging_delete_pending_gpu_meshes.erase(id);
    gpu_meshes.erase(id);
}

render::gpu_mesh* __detail::__render_gpu_mesh_get(const uuid& id) {
    std::lock_guard<std::mutex> lk(sync_mutex);
    auto it = gpu_meshes.find(id);
    if (it == gpu_meshes.end()) return nullptr;
    return &it->second;
}

void render::record_mesh_buffer_uploads(vk::CommandBuffer& cmd) {
    std::lock_guard<std::mutex> lk(sync_mutex);
    for (auto& [id, mesh] : upload_pending_gpu_meshes) {
        mesh->record_mesh_upload(cmd);
    }
    upload_pending_gpu_meshes.clear();
}

void render::destroy_staging_mesh_buffers() {
    for (auto& [id, mesh] : staging_delete_pending_gpu_meshes) {
        mesh->destroy_staging_buffers();
    }
    staging_delete_pending_gpu_meshes.clear();
}