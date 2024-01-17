#pragma once

#include <render/vulkan.h>

namespace element {
    namespace events {
        
        struct render_sync {
            vk::CommandBuffer cmd;
        };

        struct render_idle {
            /* data */
        };

    } // namespace events
} // namespace element
