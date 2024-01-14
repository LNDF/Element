#pragma once

#include <render/vulkan_command_buffer.h>
#include <vector>

namespace element {
    namespace render {
        void record_renderer_sync(vk::CommandBuffer cmd);
        void destroy_staging_buffers();
    } //namespace render
} //namespace element