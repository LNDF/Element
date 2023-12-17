#include "render_sync.h"

#include <render/global_data.h>
#include <render/mesh_manager.h>
#include <render/material.h>
#include <render/scene_data_manager.h>

using namespace element;

void render::record_renderer_sync(vk::CommandBuffer cmd) {
    global_data::record_sync(cmd);
    record_sync_scene_render_data(cmd);
    record_sync_materials(cmd);
    record_mesh_buffer_uploads(cmd);
}

void render::destroy_staging_buffers() {
    destroy_staging_mesh_buffers();
}