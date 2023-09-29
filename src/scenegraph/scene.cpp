#include "scene.h"

#include <scenegraph/node_type_info.h>
#include <scenegraph/node_storage.h>

using namespace element;

scenegraph::scene::scene(const std::string& name) : id(uuid::null()) name(name) {}

scenegraph::scene::scene(std::string&& name) : name(std::move(name)) {}

scenegraph::scene::~scene() {
    for (auto& [type, storage] : node_storages) {
        delete storage;
    }
}

scenegraph::node_storage_base* scenegraph::scene::get_storage(std::type_index type) {
    auto it = node_storages.find(type);
    if (it != node_storages.end()) {
        return it->second;
    }
    node_storage_base* storage = get_node_type_info(type).storage_factory(id.is_null() ? nullptr : this);
    node_storages.try_emplace(type, storage);
    return storage;
}

scenegraph::scene::init_scene(const uuid& id) {
    this->id = id;
    for (auto& [type, storage] : node_storages) {
        storage->init_scene(this);
    }
}