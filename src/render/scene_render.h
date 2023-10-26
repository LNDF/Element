#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace render {
        
        void init_scene_renderer(vk::SurfaceFormatKHR format);
        void cleanup_scene_rrenderer();

    } // namespace render
} // namespace element
