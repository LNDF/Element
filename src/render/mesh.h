#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace element {
    namespace render {
        struct vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 tex_coords;
        };

        struct mesh {
            std::vector<vertex> vertices;
            std::vector<std::uint32_t> indices;
        };
    } // namespace render
    
} // namespace element
