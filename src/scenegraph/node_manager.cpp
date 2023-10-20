#include "node_manager.h"

static std::unordered_map<element::uuid, element::scenegraph::node_storage_base*> storage_map;
static std::uint32_t cache_number = 0;

using namespace element;

scenegraph::node_storage_base* scenegraph::get_node_storage_mapping(const uuid& id) {
    auto it = storage_map.find(id);
    if (it == storage_map.end()) {
        return nullptr;
    }
    return it->second;
}

void scenegraph::update_node_storage_mapping(const uuid& id, node_storage_base* storage) {
    if (!storage_map.insert_or_assign(id, storage).second) {
        cache_number++;
    }
}

void scenegraph::delete_node_storage_mapping(const uuid& id) {
    auto it = storage_map.find(id);
    if (it != storage_map.end()) {
        storage_map.erase(it);
        cache_number++;
    }
}

std::uint32_t scenegraph::get_node_manager_cache_number() {
    return cache_number;
}