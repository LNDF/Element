#include "mesh_node.h"

#include <scenegraph/scene.h>
#include <render/scene_data_manager.h>

using namespace element;

void scenegraph::mesh_node::setup() {
    render_data = render::get_scene_render_data(get_owner_scene()->get_id());
    render_data->register_node(*this);
    registered = true;
}

void scenegraph::mesh_node::cleanup() {
    render_data->unregister_node(get_id());
    registered = false;
}

void scenegraph::mesh_node::enable() {
    render_data->enable_node(*this);
}

void scenegraph::mesh_node::disable() {
    render_data->disable_node(get_id());
}

void scenegraph::mesh_node::set_mesh(const uuid& mesh) {
    this->mesh = mesh;
    if (registered) {
        render_data->register_node(*this);
    }
}

void scenegraph::mesh_node::set_material(const uuid& material) {
    this->material = material;
    if (registered) {
        render_data->register_node(*this);
    }
}

ELM_REGISTER_NODE_TYPE(element::scenegraph::mesh_node, "Mesh")