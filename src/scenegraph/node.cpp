#include "node.h"

#include <scenegraph/node_storage.h>
#include <scenegraph/node_manager.h>
#include <scenegraph/scene.h>

using namespace element::scenegraph;

node::node() : id(uuid::null()), transf(node_ref(nullptr)) {}

node::~node() {}

node::node(const uuid& id, const std::string& name, node_ref& parent) 
    : name(name), id(id), transf(node_ref(id)), parent(parent), owner_scene(parent->get_owner_scene()) {}

node::node(const uuid& id, std::string&& name, node_ref& parent) 
    : name(std::move(name)), id(id), transf(node_ref(id)), parent(parent), owner_scene(parent->get_owner_scene()) {}

node::node(const uuid& id, const std::string& name, scene* owner_scene) 
    : name(name), id(id), transf(node_ref(id)), parent(uuid::null()), owner_scene(owner_scene) {}

node::node(const uuid& id, std::string&& name, scene* owner_scene) 
    : name(std::move(name)), id(id), transf(node_ref(id)), parent(uuid::null()), owner_scene(owner_scene) {}

void node::destroy() {
    for (node_ref& child : children) {
        child->destroy();
    }
    if (owner_scene != nullptr) {
        node_storage_base* storage = owner_scene->get_storage(std::type_index(typeid(this)));
        storage->delete_node(this->id);
    }
}

void node::add_child(const node_ref& child) {
    //TODO: insert into scene
}

node_ref node::add_child(std::type_index type, const std::string& name) {
    if (owner_scene != nullptr) {
        uuid new_id;
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.push_back(new_id);
        storage->emplace_node(new_id, name, this_ref);
        return new_id;
    }
    return nullptr;
}

node_ref node::add_child(std::type_index type, std::string&& name) {
    if (owner_scene != nullptr) {
        uuid new_id;
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.push_back(new_id);
        storage->emplace_node(new_id, std::move(name), this_ref);
        return new_id;
    }
    return nullptr;
}

void node::set_enabled(bool enabled) {
    if (enabled && !this->enabled) {
        enable();
    } else (!enabled && this->enabled) {
        disable();
    }
    this->enable = enable;
}

void node::setup() {}

void node::cleanup() {}

void node::enable() {}

void node::disable() {}

ELM_REGISTER_NODE_TYPE(element::scenegraph::node, "Node")