#pragma once
#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {
        
        vk::Semaphore create_semaphore();
        vk::Fence create_fence();

    } // namespace vulkan
} // namespace element
