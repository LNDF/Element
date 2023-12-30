#include "mesh_node.h"

#include <scenegraph/scene.h>
#include <render/scene_data_manager.h>

using namespace element;

void scenegraph::mesh_node::render_register() {
    if (render_data == nullptr) return;
    if (!mesh.is_null() && !material.is_null()) {
        render_data->register_node(*this);
        registered = true;
    } else if (registered) {
        render_unregister();
    }
}

void scenegraph::mesh_node::render_unregister() {
    if (registered) {
        render_data->unregister_node(get_id());
        registered = false;
    }
}

void scenegraph::mesh_node::setup() {
    render_data = render::get_scene_render_data(get_owner_scene()->get_id());
    render_register();
}

void scenegraph::mesh_node::cleanup() {
    render_unregister();
}

void scenegraph::mesh_node::enable() {
    if (registered) render_data->enable_node(*this);
}

void scenegraph::mesh_node::disable() {
    if (registered) render_data->disable_node(get_id());
}

void scenegraph::mesh_node::set_mesh(const uuid& mesh) {
    this->mesh = mesh;
    if (is_enabled()) {
        render_register();
        enable();
    }
}

void scenegraph::mesh_node::set_material(const uuid& material) {
    this->material = material;
    if (is_enabled()) {
        render_register();
        enable();
    }
}

ELM_REGISTER_NODE_TYPE(element::scenegraph::mesh_node, "Mesh")