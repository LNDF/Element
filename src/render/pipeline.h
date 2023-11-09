#pragma once

#include <utils/uuid.h>

namespace element {
    namespace render {
        
        struct pipeline_data {
            uuid vert_id;
            uuid frag_id;
            bool backface_culling;
            bool transparent;
        };

    } // namespace render
} // namespace element
