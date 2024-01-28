#include "node.h"

#include <scenegraph/node_storage.h>
#include <scenegraph/node_manager.h>
#include <scenegraph/scene.h>
#include <algorithm>

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

void node::__destroy() {
    for (node_ref& child : children) {
        child->__destroy();
    }
    node_storage_base* storage = owner_scene->get_storage(std::type_index(typeid(*this)));
    storage->delete_node(this->id);
}

void node::destroy() {
    if (owner_scene != nullptr) {
        if (parent != nullptr) {
            auto& parent_children = parent->children;
            node_ref this_ref = this;
            parent_children.erase(std::remove(parent_children.begin(), parent_children.end(), this_ref), parent_children.end());
        }
        __destroy();
    }
}

void node::add_child(const node_ref& child, std::uint32_t index) {
    if (owner_scene != nullptr && owner_scene == child->owner_scene) {
        children.insert(children.begin() + index, child);
        if (child->parent != nullptr) {
            auto& parent_children = child->parent->children;
            parent_children.erase(std::remove(parent_children.begin(), parent_children.end(), child), parent_children.end());
        }
        child->parent = this;
    }
}

node_ref node::add_child(std::type_index type, const std::string& name, std::uint32_t index) {
    if (owner_scene != nullptr) {
        uuid new_id;
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.insert(children.begin() + index, new_id);
        storage->emplace_node(new_id, name, this_ref);
        return new_id;
    }
    return nullptr;
}

node_ref node::add_child(std::type_index type, std::string&& name, std::uint32_t index) {
    if (owner_scene != nullptr) {
        uuid new_id;
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.insert(children.begin() + index, new_id);
        storage->emplace_node(new_id, std::move(name), this_ref);
        return new_id;
    }
    return nullptr;
}

void node::move_child(std::uint32_t old_index, std::uint32_t new_index) {
    if (old_index > new_index) {
        std::rotate(children.rend() - old_index - 1, children.rend() - old_index, children.rend() - new_index);
    } else {
        std::rotate(children.begin() + old_index, children.begin() + old_index + 1, children.begin() + new_index + 1);
    }
}

void node::set_enabled(bool enabled) {
    if (owner_scene != nullptr) {
        if (enabled && !this->enabled) {
            enable();
        } else if (!enabled && this->enabled) {
            disable();
        }
    }
    this->enabled = enabled;
}

void node::setup() {}

void node::cleanup() {}

void node::enable() {}

void node::disable() {}

ELM_REGISTER_NODE_TYPE(element::scenegraph::node, "Node")