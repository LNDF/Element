#include "transform.h"

#include <scenegraph/node.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace element;

static glm::vec3 position_from_mat4(const glm::mat4& matrix) {
    return matrix[3];
}

static glm::vec3 scale_from_mat4(const glm::mat4& matrix) {
    glm::vec3 scale;
    for (glm::length_t i = 0; i < 3; ++i) {
        scale[i] = glm::length(matrix[i]);
    }
    return scale;
}

static glm::quat rotation_from_mat4(const glm::mat4& matrix, const glm::vec3& scale) {
    const glm::mat3 tmp(glm::vec3(matrix[0]) / scale[0],
                        glm::vec3(matrix[1]) / scale[1],
                        glm::vec3(matrix[2]) / scale[2]);
    return glm::quat_cast(tmp);
}

scenegraph::transform::transform(const node_ref& owner) : owner(owner) {}

void scenegraph::transform::clear_cache() const {
    if (cache_parent_matrix.has_value() ||
            cache_parent_matrix_inv.has_value() ||
            cache_world_matrix.has_value() ||
            cache_world_matrix_inv.has_value() ||
            cache_world_position.has_value() ||
            cache_world_scale.has_value() ||
            cache_world_rotation.has_value()) {
        cache_parent_matrix = std::nullopt;
        cache_parent_matrix_inv = std::nullopt;
        cache_world_matrix = std::nullopt;
        cache_world_matrix_inv = std::nullopt;
        cache_world_position = std::nullopt;
        cache_world_scale = std::nullopt;
        cache_world_rotation = std::nullopt;
        ++cache_number;
        for (const node_ref& child : owner->get_children()) {
            child->get_transform().clear_cache();
        }
    }
}

void scenegraph::transform::clear_local_cache() const {
    if (cache_local_matrix.has_value() ||
            cache_world_matrix.has_value() ||
            cache_world_matrix_inv.has_value() ||
            cache_world_position.has_value() ||
            cache_world_scale.has_value() ||
            cache_world_rotation.has_value()) {
        cache_local_matrix = std::nullopt;
        cache_world_matrix = std::nullopt;
        cache_world_matrix_inv = std::nullopt;
        cache_world_position = std::nullopt;
        cache_world_scale = std::nullopt;
        cache_world_rotation = std::nullopt;
        ++cache_number;
        for (const node_ref& child : owner->get_children()) {
            child->get_transform().clear_cache();
        }
    }
}

const glm::mat4& scenegraph::transform::get_parent_matrix() const {
    if (!cache_parent_matrix.has_value()) {
        const node_ref& parent = owner->get_parent();
        if (parent == nullptr) {
            const transform& parent_trans = parent->get_transform();
            const glm::mat4& parent_parent = parent_trans.get_parent_matrix();
            const glm::mat4& parent_local = parent_trans.get_matrix();
            cache_parent_matrix = parent_parent * parent_local;
        } else {
            cache_parent_matrix = glm::identity<glm::mat4>();
        }
    }
    return cache_parent_matrix.value();
}

const glm::mat4& scenegraph::transform::get_parent_matrix_inv() const {
    if (!cache_parent_matrix_inv.has_value()) {
        cache_parent_matrix_inv = glm::inverse(get_parent_matrix());
    }
    return cache_parent_matrix_inv.value();
}

const glm::mat4& scenegraph::transform::get_world_matrix_inv() const {
    if (!cache_world_matrix_inv.has_value()) {
        cache_world_matrix_inv = glm::inverse(get_world_matrix());
    }
    return cache_world_matrix_inv.value();
}

const glm::mat4& scenegraph::transform::get_matrix() const {
    if (!cache_local_matrix.has_value()) {
        glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), position);
        glm::mat4 rot = glm::mat4_cast(rotation);
        glm::mat4 scl = glm::scale(glm::identity<glm::mat4>(), scale);
        cache_local_matrix = translate * rot * scl;
    }
    return cache_local_matrix.value();
}

const glm::mat4& scenegraph::transform::get_world_matrix() const {
    if (!cache_world_matrix.has_value()) {
        const glm::mat4& parent = get_parent_matrix();
        const glm::mat4& local = get_matrix();
        cache_world_matrix = parent * local;
    }
    return cache_world_matrix.value();
}

void scenegraph::transform::set_position(const glm::vec3& position) {
    this->position = position;
    clear_local_cache();
}

void scenegraph::transform::set_scale(const glm::vec3& scale) {
    this->scale = scale;
    clear_local_cache();
}

void scenegraph::transform::set_rotation(const glm::quat& rotation) {
    this->rotation = rotation;
    clear_local_cache();
}

const glm::vec3& scenegraph::transform::get_world_position() const {
    if (!cache_world_position.has_value()) {
        cache_world_position = position_from_mat4(get_world_matrix());
    }
    return cache_world_position.value();
}

const glm::vec3& scenegraph::transform::get_world_scale() const {
    if (!cache_world_scale.has_value()) {
        cache_world_scale = scale_from_mat4(get_world_matrix());
    }
    return cache_world_scale.value();
}

const glm::quat& scenegraph::transform::get_world_rotation() const {
    if (!cache_world_rotation.has_value()) {
        cache_world_rotation = rotation_from_mat4(get_world_matrix(), get_world_scale());
    }
    return cache_world_rotation.value();
}

void scenegraph::transform::set_world_position(const glm::vec3& position) {
    this->position = get_parent_matrix_inv() * glm::vec4(position, 1.0f);
    clear_local_cache();
}

void scenegraph::transform::set_world_scale(const glm::vec3& scale) {
    this->scale = scale / scale_from_mat4(get_parent_matrix());
    clear_local_cache();
}

void scenegraph::transform::set_world_rotation(const glm::quat& rotation) {
    const glm::mat4& parent = get_parent_matrix();
    this->rotation = glm::conjugate(rotation_from_mat4(parent, scale_from_mat4(parent))) * rotation;
    clear_local_cache();
}

glm::vec3 scenegraph::transform::get_front() const {
    return get_matrix() * glm::vec4(0, 0, -1, 1);
}

glm::vec3 scenegraph::transform::get_right() const {
    return get_matrix() * glm::vec4(1, 0, 0, 1);
}

glm::vec3 scenegraph::transform::get_up() const {
    return get_matrix() * glm::vec4(0, 1, 0, 1);
}

glm::vec3 scenegraph::transform::get_world_front() const {
    return get_world_matrix() * glm::vec4(0, 0, -1, 1);
}

glm::vec3 scenegraph::transform::get_world_right() const {
    return get_world_matrix() * glm::vec4(1, 0, 0, 1);
}

glm::vec3 scenegraph::transform::get_world_up() const {
    return get_world_matrix() * glm::vec4(0, 1, 0, 1);
}

void scenegraph::transform::rotate_axis(const glm::vec3& axis, float angle) {
    set_rotation(glm::rotate(glm::identity<glm::quat>(), angle, axis) * glm::normalize(get_rotation()));
}

void scenegraph::transform::rotate_world_axis(const glm::vec3& axis, float angle) {
    set_world_rotation(glm::rotate(glm::identity<glm::quat>(), angle, axis) * glm::normalize(get_world_rotation()));
}

void scenegraph::transform::look_at(const glm::vec3& front, const glm::vec3& up) {
    set_rotation(glm::quatLookAt(front, up));
}

void scenegraph::transform::world_look_at(const glm::vec3& front, const glm::vec3& up) {
    set_world_rotation(glm::quatLookAt(front, up));
}

void scenegraph::transform::rotate_euler(const glm::vec3& angles) {
    set_rotation(glm::quat(angles) * get_rotation());
}

void scenegraph::transform::world_rotate_euler(const glm::vec3& angles) {
    set_world_rotation(glm::quat(angles) * get_world_rotation());
}

scenegraph::transform_watcher::transform_watcher(const transform& trans)
    : node(trans.get_owner()) {}

const scenegraph::transform& scenegraph::transform_watcher::get_transform() const {
    return node->get_transform();
}

bool scenegraph::transform_watcher::has_updated() const {
    if (cache_number != get_transform().cache_number) {
        cache_number = get_transform().cache_number;
        return true;
    }
    return false;
}