#include "resource_sync.h"

#include <render/mesh_manager.h>

using namespace element;

void render::record_sync_materials(vk::CommandBuffer& cmd, const std::vector<gpu_material*>& used_materials) {
    for (gpu_material* mat : used_materials) {
        mat->record_sync_if_meeded(cmd);
    }
}

void render::record_resource_sync(vk::CommandBuffer cmd, const std::vector<gpu_material*>& used_materials) {
    record_sync_materials(cmd, used_materials);
    record_mesh_buffer_uploads(cmd);
}

void render::delete_staging_buffers() {
    delete_staging_mesh_buffers();
}