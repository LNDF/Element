#include "node.h"

#include <scenegraph/node_storage.h>
#include <scenegraph/node_manager.h>
#include <scenegraph/scene.h>

using namespace element::scenegraph;

node::node() : id(uuid::null()) {}

node::~node() {}

node::node(const uuid& id, const std::string& name, const node_ref& parent) 
    : name(name), id(id), parent(parent), owner_scene(parent->get_owner_scene()) {}

node::node(const uuid& id, std::string&& name, const node_ref& parent) 
    : name(std::move(name)), id(id), parent(parent), owner_scene(parent->get_owner_scene()) {}

node::node(const uuid& id, const std::string& name, scene* owner_scene) 
    : name(name), id(id), parent(id), owner_scene(owner_scene) {}

node::node(const uuid& id, std::string&& name, scene* owner_scene) 
    : name(std::move(name)), id(id), parent(id), owner_scene(owner_scene) {}

void node::destroy() {
    for (const node_ref& child : children) {
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
        uuid new_id = get_new_node_id();
        node_storage_base* storage = owner_scene->get_storage(type);
        children.push_back(new_id);
        storage->emplace_node(new_id, name, this);
        return new_id;
    }
    return nullptr;
}

node_ref node::add_child(std::type_index type, std::string&& name) {
    if (owner_scene != nullptr) {
        uuid new_id = get_new_node_id();
        node_storage_base* storage = owner_scene->get_storage(type);
        children.push_back(new_id);
        storage->emplace_node(new_id, std::move(name), this);
        return new_id;
    }
    return nullptr;
}

void node::node_setup() {}

void node::node_cleanup() {}

ELM_REGISTER_NODE_TYPE(element::scenegraph::node, "Node")