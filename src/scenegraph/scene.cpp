#include "scene.h"

#include <scenegraph/node_type_info.h>
#include <scenegraph/node.h>
#include <scenegraph/node_storage.h>
#include <scenegraph/node_manager.h>

using namespace element;

scenegraph::scene::scene() : id(uuid::null()) {}

scenegraph::scene::~scene() {
    
}

scenegraph::node_storage_base* scenegraph::scene::get_storage(std::type_index type) {
    auto it = node_storages.find(type);
    if (it != node_storages.end()) {
        return it->second.get();
    }
    node_storage_base* storage = get_node_type_info(type).storage_factory(id.is_null() ? nullptr : this);
    node_storages.try_emplace(type, storage);
    return storage;
}

void scenegraph::scene::init_scene(const uuid& id) {
    this->id = id;
    for (auto& [type, storage] : node_storages) {
        storage->init_scene(this);
    }
    if (root_node != nullptr) return;
    uuid root_node_id = get_new_node_id();
    node_storage_base* storage = get_storage(std::type_index(typeid(node)));
    storage->emplace_root(root_node_id);
    this->root_node = root_node_id;
}