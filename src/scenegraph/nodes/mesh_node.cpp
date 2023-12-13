#include "mesh_node.h"

using namespace element;

void scenegraph::mesh_node::setup() {
    //TODO: register
    registered = true;
}

void scenegraph::mesh_node::cleanup() {
    //TODO: unregister
    registered = false;
}

void scenegraph::mesh_node::enable() {
    //TODO: enable mesh
}

void scenegraph::mesh_node::disable() {
    //TODO: disable mesh
}

void scenegraph::mesh_node::set_mesh(const uuid& mesh) {
    this->mesh = mesh;
    if (registered) {
        //TODO: update mesh
    }
}

void scenegraph::mesh_node::set_material(const uuid& material) {
    this->material = material;
    if (registered) {
        //TODO: update material
    }
}

ELM_REGISTER_NODE_TYPE(element::scenegraph::node, "Mesh node")