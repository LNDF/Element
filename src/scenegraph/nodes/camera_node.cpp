#include "camera_node.h"



using namespace element;

glm::mat4 scenegraph::camera_node::get_view_matrix() const {
    const glm::quat& rot = glm::conjugate(get_transform().get_world_rotation());
    const glm::vec3& pos = get_transform().get_world_position() * -1.0f;
    const glm::vec3& scale = get_transform().get_world_scale();
    glm::mat4 rot_mat = glm::mat4_cast(rot);
    glm::mat4 pos_mat = glm::translate(glm::identity<glm::mat4>(), pos);
    glm::mat4 scale_mat = glm::scale(glm::identity<glm::mat4>(), scale);
    return scale_mat * rot_mat * pos_mat;
}

glm::mat4 scenegraph::camera_node::get_projection_matrix(std::uint32_t width, std::uint32_t height) const {
    glm::mat4 proj = glm::perspectiveRH_ZO(fov, (float) width / (float) height, near_plane, far_plane);
    proj[1][1] *= -1;
    return proj;
}

glm::mat4 scenegraph::camera_node::get_view_projection_matrix(std::uint32_t width, std::uint32_t height) const {
    return get_projection_matrix(width, height) * get_view_matrix();
}

ELM_REGISTER_NODE_TYPE(element::scenegraph::camera_node, "Camera")