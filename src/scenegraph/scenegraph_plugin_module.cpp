#include <scenegraph/node.h>
#include <scenegraph/node_ref.h>
#include <scenegraph/nodes/camera_node.h>
#include <scenegraph/nodes/camera_node_ref.h>
#include <scenegraph/nodes/mesh_node.h>
#include <scenegraph/nodes/mesh_node_ref.h>
#include <scenegraph/transform.h>
#include <scenegraph/scene.h>
#include <scenegraph/scene_manager.h>
#include <plugins/module.h>
#include <utils/uuid.h>
#include <element/defs.h>
#include <element/types/scenegraph.h>
#include <element/modules/scenegraph.h>
#include <unordered_map>
#include <utility>

using namespace element;

static std::unordered_map<uuid, std::pair<scenegraph::node_ref, std::uint32_t>> node_ref_map;

ELM_BEGIN_C

static void scene_load(element_uuid_t id) {
    uuid& s = reinterpret_cast<uuid&>(id);
    scenegraph::load_scene(s);
}

static void scene_unload(element_uuid_t id) {
    uuid& s = reinterpret_cast<uuid&>(id);
    scenegraph::destroy_scene(s);
}

static element_scene_t scene_get(element_uuid_t id) {
    uuid& s = reinterpret_cast<uuid&>(id);
    return reinterpret_cast<element_scene_t>(scenegraph::get_scene(s));
}

static element_uuid_t scene_get_root_node(element_scene_t scene) {
    scenegraph::scene* s = reinterpret_cast<scenegraph::scene*>(scene);
    uuid id = s->get_root_node().get_id();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static element_uuid_t scene_get_default_camera(element_scene_t scene) {
    scenegraph::scene* s = reinterpret_cast<scenegraph::scene*>(scene);
    uuid id = s->get_default_camera().get_id();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static void scene_set_default_camera(element_scene_t scene, element_uuid_t camera) {
    scenegraph::scene* s = reinterpret_cast<scenegraph::scene*>(scene);
    uuid& c = reinterpret_cast<uuid&>(camera);
    s->set_default_camera(scenegraph::camera_node_ref(c));
}

static element_node_t node_get(element_uuid_t id) {
    uuid& n = reinterpret_cast<uuid&>(id);
    auto it = node_ref_map.find(n);
    if (it != node_ref_map.end()) {
        ++it->second.second;
        return &it->second.first;
    }
    return &node_ref_map.insert({n, {scenegraph::node_ref(n), 1}}).first->second.first;
}

static void node_free(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    auto it = node_ref_map.find(n->get_id());
    if (it != node_ref_map.end()) {
        if (--it->second.second == 0) {
            node_ref_map.erase(it);
        }
    }
}

static element_uuid_t node_get_uuid(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    uuid id = n->get_id();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static void node_set_enabled(element_node_t node, int enabled) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    n->get_node()->set_enabled(enabled);
}

static int node_is_enabled(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    return n->get_node()->is_enabled();
}

static void node_set_name(element_node_t node, const char* name) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    n->get_node()->set_name(name);
}

static const char* node_get_name(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    return n->get_node()->get_name().c_str();
}

static uint32_t node_get_children(element_node_t node, element_uuid_t* children) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    auto& c = n->get_node()->get_children();
    if (!children) {
        return c.size();
    }
    uint32_t i = 0;
    for (auto& child : c) {
        uuid id = child.get_id();
        children[i++] = *reinterpret_cast<element_uuid_t*>(&id);
    }
    return i;
}

static element_uuid_t node_get_parent(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    uuid id = n->get_node()->get_parent().get_id();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static uint32_t node_transform_get_cache_number(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    return n->get_node()->get_transform().get_cache_number();
}

static element_mat4_t node_transform_get_matrix(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::mat4 m = n->get_node()->get_transform().get_matrix();
    return *reinterpret_cast<element_mat4_t*>(&m);
}

static element_mat4_t node_transform_get_world_matrix(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::mat4 m = n->get_node()->get_transform().get_world_matrix();
    return *reinterpret_cast<element_mat4_t*>(&m);
}

static element_vec3_t node_transform_get_position(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_position();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_scale(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_scale();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_quat_t node_transform_get_rotation(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::quat q = n->get_node()->get_transform().get_rotation();
    return *reinterpret_cast<element_quat_t*>(&q);
}

static void node_transform_set_position(element_node_t node, element_vec3_t position) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& p = reinterpret_cast<glm::vec3&>(position);
    n->get_node()->get_transform().set_position(p);
}

static void node_transform_set_scale(element_node_t node, element_vec3_t scale) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& s = reinterpret_cast<glm::vec3&>(scale);
    n->get_node()->get_transform().set_scale(s);
}

static void node_transform_set_rotation(element_node_t node, element_quat_t rotation) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::quat& r = reinterpret_cast<glm::quat&>(rotation);
    n->get_node()->get_transform().set_rotation(r);
}

static element_vec3_t node_transform_get_world_position(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_position();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_scale(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_scale();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_quat_t node_transform_get_world_rotation(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::quat q = n->get_node()->get_transform().get_world_rotation();
    return *reinterpret_cast<element_quat_t*>(&q);
}

static void node_transform_set_world_position(element_node_t node, element_vec3_t position) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& p = reinterpret_cast<glm::vec3&>(position);
    n->get_node()->get_transform().set_world_position(p);
}

static void node_transform_set_world_scale(element_node_t node, element_vec3_t scale) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& s = reinterpret_cast<glm::vec3&>(scale);
    n->get_node()->get_transform().set_world_scale(s);
}

static void node_transform_set_world_rotation(element_node_t node, element_quat_t rotation) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::quat& r = reinterpret_cast<glm::quat&>(rotation);
    n->get_node()->get_transform().set_world_rotation(r);
}

static element_vec3_t node_transform_get_front(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_front();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_right(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_right();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_up(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_up();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_back(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_back();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_left(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_left();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_down(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_down();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_front(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_front();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_right(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_right();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_up(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_up();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_back(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_back();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static element_vec3_t node_transform_get_world_left(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_left();
    return *reinterpret_cast<element_vec3_t*>(&v);
}


static element_vec3_t node_transform_get_world_down(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_down();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static void node_transform_rotate_axis(element_node_t node, element_vec3_t axis, float angle) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(axis);
    n->get_node()->get_transform().rotate_axis(a, angle);
}

static void node_transform_rotate_world_axis(element_node_t node, element_vec3_t axis, float angle) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(axis);
    n->get_node()->get_transform().rotate_world_axis(a, angle);
}

static void node_transform_look_at(element_node_t node, element_vec3_t front, element_vec3_t up) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& f = reinterpret_cast<glm::vec3&>(front);
    glm::vec3& u = reinterpret_cast<glm::vec3&>(up);
    n->get_node()->get_transform().look_at(f, u);
}

static void node_transform_world_look_at(element_node_t node, element_vec3_t front, element_vec3_t up) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& f = reinterpret_cast<glm::vec3&>(front);
    glm::vec3& u = reinterpret_cast<glm::vec3&>(up);
    n->get_node()->get_transform().world_look_at(f, u);
}

static void node_transform_rotate_euler(element_node_t node, element_vec3_t angles) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(angles);
    n->get_node()->get_transform().rotate_euler(a);
}

static void node_transform_set_euler_rotation(element_node_t node, element_vec3_t angles) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(angles);
    n->get_node()->get_transform().set_euler_rotation(a);
}

static element_vec3_t node_transform_get_euler_angles(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_euler_angles();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static void node_transform_world_rotate_euler(element_node_t node, element_vec3_t angles) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(angles);
    n->get_node()->get_transform().world_rotate_euler(a);
}

static void node_transform_set_world_euler_rotation(element_node_t node, element_vec3_t angles) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3& a = reinterpret_cast<glm::vec3&>(angles);
    n->get_node()->get_transform().set_world_euler_rotation(a);
}

static element_vec3_t node_transform_get_world_euler_angles(element_node_t node) {
    scenegraph::node_ref* n = reinterpret_cast<scenegraph::node_ref*>(node);
    glm::vec3 v = n->get_node()->get_transform().get_world_euler_angles();
    return *reinterpret_cast<element_vec3_t*>(&v);
}

static float node_camera_get_near_plane(element_node_t node) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    return n->get_node()->get_near_plane();
}

static void node_camera_set_near_plane(element_node_t node, float near_plane) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    n->get_node()->set_near_plane(near_plane);
}

static float node_camera_get_far_plane(element_node_t node) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    return n->get_node()->get_far_plane();
}

static void node_camera_set_far_plane(element_node_t node, float far_plane) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    n->get_node()->set_far_plane(far_plane);
}

static float node_camera_get_fov(element_node_t node) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    return n->get_node()->get_fov();
}

static void node_camera_set_fov(element_node_t node, float fov) {
    scenegraph::camera_node_ref* n = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    n->get_node()->set_fov(fov);
}

static element_uuid_t node_mesh_get_mesh(element_node_t node) {
    scenegraph::mesh_node_ref* n = reinterpret_cast<scenegraph::mesh_node_ref*>(node);
    uuid id = n->get_node()->get_mesh();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static void node_mesh_set_mesh(element_node_t node, element_uuid_t mesh) {
    scenegraph::mesh_node_ref* n = reinterpret_cast<scenegraph::mesh_node_ref*>(node);
    uuid& m = reinterpret_cast<uuid&>(mesh);
    n->get_node()->set_mesh(m);
}

static element_uuid_t node_mesh_get_material(element_node_t node) {
    scenegraph::mesh_node_ref* n = reinterpret_cast<scenegraph::mesh_node_ref*>(node);
    uuid id = n->get_node()->get_material();
    return *reinterpret_cast<element_uuid_t*>(&id);
}

static void node_mesh_set_material(element_node_t node, element_uuid_t material) {
    scenegraph::mesh_node_ref* n = reinterpret_cast<scenegraph::mesh_node_ref*>(node);
    uuid& m = reinterpret_cast<uuid&>(material);
    n->get_node()->set_material(m);
}

static element_module_scenegraph_t module_scenegraph = {
    scene_load,
    scene_unload,
    scene_get,
    scene_get_root_node,
    scene_get_default_camera,
    scene_set_default_camera,
    node_get,
    node_free,
    node_get_uuid,
    node_set_enabled,
    node_is_enabled,
    node_set_name,
    node_get_name,
    node_get_children,
    node_get_parent,
    node_transform_get_cache_number,
    node_transform_get_matrix,
    node_transform_get_world_matrix,
    node_transform_get_position,
    node_transform_get_scale,
    node_transform_get_rotation,
    node_transform_set_position,
    node_transform_set_scale,
    node_transform_set_rotation,
    node_transform_get_world_position,
    node_transform_get_world_scale,
    node_transform_get_world_rotation,
    node_transform_set_world_position,
    node_transform_set_world_scale,
    node_transform_set_world_rotation,
    node_transform_get_front,
    node_transform_get_right,
    node_transform_get_up,
    node_transform_get_back,
    node_transform_get_left,
    node_transform_get_down,
    node_transform_get_world_front,
    node_transform_get_world_right,
    node_transform_get_world_up,
    node_transform_get_world_back,
    node_transform_get_world_left,
    node_transform_get_world_down,
    node_transform_rotate_axis,
    node_transform_rotate_world_axis,
    node_transform_look_at,
    node_transform_world_look_at,
    node_transform_rotate_euler,
    node_transform_set_euler_rotation,
    node_transform_get_euler_angles,
    node_transform_world_rotate_euler,
    node_transform_set_world_euler_rotation,
    node_transform_get_world_euler_angles,
    node_camera_get_near_plane,
    node_camera_set_near_plane,
    node_camera_get_far_plane,
    node_camera_set_far_plane,
    node_camera_get_fov,
    node_camera_set_fov,
    node_mesh_get_mesh,
    node_mesh_set_mesh,
    node_mesh_get_material,
    node_mesh_set_material
};

ELM_END_C

ELM_REGISTER_MODULE("element.scenegraph", &module_scenegraph)