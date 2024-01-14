#pragma once

#include <render/mesh.h>
#include <render/vulkan_command_buffer.h>
#include <resource/async_loader.h>
#include <resource/referenced_manager.h>
#include <serialization/render/mesh.h>

namespace element {
    namespace __detail {
        void __render_gpu_mesh_resource_loaded(const uuid& id, const render::mesh& mesh);
        void __render_gpu_mesh_destroy(const uuid& id);
        render::gpu_mesh* __render_gpu_mesh_get(const uuid& id);
    } //namespace __detail

    namespace render {
        using gpu_mesh_manager = resource::referenced_resource_manager<gpu_mesh, resource::async_load<mesh, __detail::__render_gpu_mesh_resource_loaded>, __detail::__render_gpu_mesh_destroy, __detail::__render_gpu_mesh_get>;

        void record_mesh_buffer_uploads(vk::CommandBuffer& cmd);
        void destroy_staging_mesh_buffers();
    } //namespace render
} //namespace element