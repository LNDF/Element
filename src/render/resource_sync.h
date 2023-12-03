#pragma once

#include <graphics/vulkan_command_buffer.h>
#include <render/material.h>
#include <vector>

namespace element {
    namespace render {
        void record_sync_materials(vk::CommandBuffer& cmd, const std::vector<gpu_material*>& used_materials);
        void record_resource_sync(vk::CommandBuffer cmd, const std::vector<gpu_material*>& used_materials);
        void delete_staging_buffers();
    } //namespace render
} //namespace element