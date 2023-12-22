#pragma once

#include <scenegraph/node_ref.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cinttypes>
#include <optional>

namespace element {
    namespace scenegraph {
        
        class transform_watcher;

        class transform {
            private:
                friend class transform_watcher;

                mutable std::optional<glm::mat4> cache_local_matrix;
                mutable std::optional<glm::mat4> cache_parent_matrix;
                mutable std::optional<glm::mat4> cache_parent_matrix_inv;
                mutable std::optional<glm::mat4> cache_world_matrix;
                mutable std::optional<glm::mat4> cache_world_matrix_inv;
                mutable std::optional<glm::vec3> cache_world_position;
                mutable std::optional<glm::vec3> cache_world_scale;
                mutable std::optional<glm::quat> cache_world_rotation;
                mutable std::uint32_t cache_number = 0;
                node_ref owner;
                glm::vec3 position;
                glm::vec3 scale;
                glm::quat rotation;
                
                void clear_cache() const;
                void clear_local_cache() const;
                const glm::mat4& get_parent_matrix() const;
                const glm::mat4& get_parent_matrix_inv() const;
                const glm::mat4& get_world_matrix_inv() const;
            public:
                transform(const node_ref& owner);

                inline const node_ref& get_owner() const {return owner;}

                const glm::mat4& get_matrix() const;
                const glm::mat4& get_world_matrix() const;
                inline const glm::vec3& get_position() const {return position;}
                inline const glm::vec3& get_scale() const {return scale;}
                inline const glm::quat& get_rotation() const {return rotation;}
                void set_position(const glm::vec3& position);
                void set_scale(const glm::vec3& scale);
                void set_rotation(const glm::quat& rotation);
                const glm::vec3& get_world_position() const;
                const glm::vec3& get_world_scale() const;
                const glm::quat& get_world_rotation() const;
                void set_world_position(const glm::vec3& position);
                void set_world_scale(const glm::vec3& scale);
                void set_world_rotation(const glm::quat& rotation);
                glm::vec3 get_front() const;
                glm::vec3 get_right() const;
                glm::vec3 get_up() const;
                inline glm::vec3 get_back() const {return get_front() * -1.0f;}
                inline glm::vec3 get_left() const {return get_right() * -1.0f;}
                inline glm::vec3 get_down() const {return get_up() * -1.0f;}
                glm::vec3 get_world_front() const;
                glm::vec3 get_world_right() const;
                glm::vec3 get_world_up() const;
                inline glm::vec3 get_world_back() const {return get_world_front() * -1.0f;}
                inline glm::vec3 get_world_left() const {return get_world_right() * -1.0f;}
                inline glm::vec3 get_world_down() const {return get_world_up() * -1.0f;}
                void rotate_axis(const glm::vec3& axis, float angle);
                void rotate_world_axis(const glm::vec3& axis, float angle);
                void look_at(const glm::vec3& front, const glm::vec3& up);
                void world_look_at(const glm::vec3& front, const glm::vec3& up);
                void rotate_euler(const glm::vec3& angles);
                void world_rotate_euler(const glm::vec3& angles);
        };

        class transform_watcher {
            private:
                node_ref node;
                mutable std::uint32_t cache_number;
            public:
                transform_watcher();
                transform_watcher(const transform& trans);
                const transform& get_transform() const;
                bool has_updated() const;
        };

    } // namespace scenegraph
} // namespace element
