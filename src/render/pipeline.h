#pragma once

#include <utils/uuid.h>
#include <graphics/vulkan.h>

namespace element {
    namespace render {
        
        enum culling_face {
            counter_clockwise,
            clockwise
        };

        struct pipeline_data {
            uuid vert_id;
            uuid frag_id;
            bool backface_culling;
            culling_face frontface;
            bool transparent;
        };

        struct pipeline {
            vk::Pipeline pipeline;
            vk::PipelineLayout layout;
        }

    } // namespace render
} // namespace element
