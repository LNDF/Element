#pragma once

#include <shader/layout.h>
#include <vector>

namespace element {
    namespace shader {
        
        struct shader_data {
            std::vector<std::uint32_t> spv;
            shader_layout layout;
        };

    } // namespace shader
} // namespace element
