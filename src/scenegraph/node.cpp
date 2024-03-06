#include "node.h"

#include <event/event.h>
#include <scenegraph/node_storage.h>
#include <scenegraph/node_manager.h>
#include <scenegraph/scene.h>
#include <scenegraph/scene_events.h>
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
        uuid this_id = id;
        events::before_node_deleted before_event;
        before_event.parent = parent;
        before_event.this_node = this;
        event_manager::send_event(before_event);
        if (parent != nullptr) {
            auto& parent_children = parent->children;
            node_ref this_ref = this;
            parent_children.erase(std::remove(parent_children.begin(), parent_children.end(), this_ref), parent_children.end());
        }
        __destroy();
        events::after_node_deleted after_event;
        after_event.parent = parent;
        after_event.id = this_id;
        event_manager::send_event(after_event);
    }
}

node_ref node::add_child(const node_ref& child, std::uint32_t index) {
    if (owner_scene != nullptr && owner_scene == child->owner_scene) {
        node_ref from = child->parent;
        events::before_node_moved before_event;
        before_event.this_node = child;
        before_event.from_parent = from;
        before_event.to_parent = this;
        before_event.dest_index = index;
        event_manager::send_event(before_event);
        glm::vec3 pos = child->transf.get_world_position();
        glm::vec3 scl = child->transf.get_world_scale();
        glm::quat rot = child->transf.get_world_rotation();
        node_ref& inserted = *(children.insert(children.begin() + index, child));
        if (inserted->parent != nullptr) {
            auto& parent_children = from->children;
            parent_children.erase(std::remove(parent_children.begin(), parent_children.end(), inserted), parent_children.end());
        }
        inserted->parent = this;
        inserted->transf.clear_cache();
        inserted->transf.set_world_position(pos);
        inserted->transf.set_world_scale(scl);
        inserted->transf.set_world_rotation(rot);
        events::after_node_moved after_event;
        after_event.from_parent = from;
        after_event.to_parent = this;
        after_event.this_node = inserted;
        event_manager::send_event(after_event);
        return inserted;
    }
    return nullptr;
}

node_ref node::add_child(std::type_index type, const std::string& name, std::uint32_t index) {
    if (owner_scene != nullptr) {
        uuid new_id;
        events::before_node_added before_event;
        before_event.id = new_id;
        before_event.index = index;
        before_event.parent = this;
        event_manager::send_event(before_event);
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.insert(children.begin() + index, new_id);
        storage->emplace_node(new_id, name, this_ref);
        events::after_node_added after_event;
        after_event.this_node = new_id;
        after_event.parent = this;
        event_manager::send_event(after_event);
        return new_id;
    }
    return nullptr;
}

node_ref node::add_child(std::type_index type, std::string&& name, std::uint32_t index) {
    if (owner_scene != nullptr) {
        uuid new_id;
        events::before_node_added before_event;
        before_event.id = new_id;
        before_event.index = index;
        before_event.parent = this;
        event_manager::send_event(before_event);
        node_storage_base* storage = owner_scene->get_storage(type);
        node_ref this_ref = this;
        children.insert(children.begin() + index, new_id);
        storage->emplace_node(new_id, std::move(name), this_ref);
        events::after_node_added after_event;
        after_event.this_node = new_id;
        after_event.parent = this;
        event_manager::send_event(after_event);
        return new_id;
    }
    return nullptr;
}

void node::move_child(std::uint32_t old_index, std::uint32_t new_index) {
    if (owner_scene == nullptr) return;
    node_ref tmp = children[old_index];
    events::before_node_moved before_event;
    before_event.from_parent = this;
    before_event.to_parent = this;
    before_event.this_node = tmp;
    before_event.dest_index = new_index;
    event_manager::send_event(before_event);
    if (old_index < new_index) --new_index;
    auto begin = children.begin();
    children.erase(begin + old_index);
    children.insert(begin + new_index, tmp);
    events::after_node_moved after_event;
    after_event.from_parent = this;
    after_event.to_parent = this;
    after_event.this_node = tmp;
    event_manager::send_event(after_event);
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