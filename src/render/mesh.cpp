#include "mesh.h"

using namespace element;

render::gpu_mesh::gpu_mesh(vulkan::device_buffer&& vertex_buffer, vulkan::device_buffer&& index_buffer)
    : vertex_buffer(std::move(vertex_buffer)), index_buffer(std::move(index_buffer)) {}

void render::gpu_mesh::record_bind_buffer(vk::CommandBuffer& cmd) {
    vk::DeviceSize offsets[1] = {0};
    vk::Buffer vertex = vertex_buffer.get_buffer(), index = index_buffer.get_buffer();
    cmd.bindVertexBuffers(0, 1, &vertex, offsets);
    cmd.bindIndexBuffer(index, 0, vk::IndexType::eUint32);
}

void render::gpu_mesh::record_mesh_upload(vk::CommandBuffer& cmd) {
    vertex_buffer.record_upload(cmd);
    index_buffer.record_upload(cmd);
}

void render::gpu_mesh::delete_staging_buffers() {
    vertex_buffer.delete_staging();
    index_buffer.delete_staging();
}