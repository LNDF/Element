#pragma once

#include <render/vulkan.h>

namespace element {
    namespace vulkan {
        
        vk::CommandBuffer create_command_buffer();
        void free_command_buffer(vk::CommandBuffer& cmd);

    } // namespace vulkan
} // namespace element
