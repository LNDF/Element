#pragma once

#include <render/shader_layout.h>
#include <vector>

namespace element {
    namespace render {
        
        struct shader {
            std::vector<std::uint32_t> spv;
            shader_layout reflect;
        };

    } // namespace shader
} // namespace element
