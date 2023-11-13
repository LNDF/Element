#pragma once

#include <render/shader_layout.h>
#include <utils/uuid.h>
#include <vector>

namespace element {
    namespace render {
        
        struct shader {
            std::vector<std::uint32_t> spv;
            shader_layout reflect;
        };

        std::vector<std::uint32_t> load_shader_spv(const uuid& id);

    } // namespace shader
} // namespace element
