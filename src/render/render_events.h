#pragma once

#include <render/vulkan.h>
#include <render/vulkan_swapchain.h>

namespace element {
    namespace events {
        
        struct render_sync {
            vk::CommandBuffer cmd;
        };

        struct render_idle {
            /* data */
        };

        struct render_suboptimal_swapchain {
            const vulkan::swapchain_info* swapchain;
        };

        struct render_skip_frame {
            const vulkan::swapchain_info* swapchain;
        };

    } // namespace events
} // namespace element
