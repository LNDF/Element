#pragma once

#include <scenegraph/node.h>
#include <glm/glm.hpp>

namespace element {
    namespace scenegraph {
        
        class camera_node : public node {
            private:
                float near_plane = 0.01f;
                float far_plane = 1000.0f;
                float fov = 45.0f;
            public:
                using node::node;

                glm::mat4 get_view_matrix() const;
                glm::mat4 get_projection_matrix(std::uint32_t width, std::uint32_t height) const;
                glm::mat4 get_view_projection_matrix(std::uint32_t width, std::uint32_t height) const;

                inline float get_near_plane() const {return near_plane;}
                inline float get_far_plane() const {return far_plane;}
                inline float get_fov() const {return fov;}
                inline void set_near_plane(float near_plane) {this->near_plane = near_plane;}
                inline void set_far_plane(float far_plane) {this->far_plane = far_plane;}
                inline void set_fov(float fov) {this->fov = fov;}
        };

    } // namespace scenegraph
} // namespace element 