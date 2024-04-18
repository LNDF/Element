#include <render/scene_render.h>
#include <render/render.h>
#include <scenegraph/nodes/camera_node_ref.h>
#include <scenegraph/scene.h>
#include <plugins/module.h>
#include <utils/uuid.h>
#include <element/defs.h>
#include <element/types/scenegraph.h>
#include <element/modules/render.h>

using namespace element;

ELM_BEGIN_C

static void select_camera(element_node_t node) {
    scenegraph::camera_node_ref* ref = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    render::get_screen_scene_renderer()->select_camera(*ref);
}

static void select_camera_and_scene(element_scene_t scene, element_node_t node) {
    scenegraph::scene* s = reinterpret_cast<scenegraph::scene*>(scene);
    scenegraph::camera_node_ref* ref = reinterpret_cast<scenegraph::camera_node_ref*>(node);
    render::get_screen_scene_renderer()->select_scene(s->get_id(), *ref);
}

static void select_scene(element_scene_t scene) {
    scenegraph::scene* s = reinterpret_cast<scenegraph::scene*>(scene);
    render::get_screen_scene_renderer()->select_scene(s->get_id());
}

static void unselect_scene_t() {
    render::get_screen_scene_renderer()->select_scene(uuid::null());
}

static element_module_render_t module_render = {
    select_camera,
    select_camera_and_scene,
    select_scene,
    unselect_scene_t
};

ELM_END_C

ELM_REGISTER_MODULE("element.render", &module_render)