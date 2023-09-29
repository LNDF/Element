#include "node_ref.h"

#include <scenegraph/node_manager.h>
#include <scenegraph/node_storage.h>
#include <scenegraph/node.h>

using namespace element::scenegraph;

node_ref::node_ref() : id(uuid::null()) {}

node_ref::node_ref(const uuid& id) : id(id) {}

node_ref::node_ref(node* node) : id(node->get_id()) {}

node_ref::node_ref(std::nullptr_t) : id(uuid::null()) {}

bool node_ref::exists() const {
    if (id.is_null()) return false;
    if (storage == nullptr || manager_cache_number != __detail::__scenegraph_node_manager_cache_number) {
        manager_cache_number = __detail::__scenegraph_node_manager_cache_number;
        auto it = __detail::__scenegraph_node_storage_map.find(id);
        if (it == __detail::__scenegraph_node_storage_map.end()) {
            storage = nullptr;
            return false;
        }
        storage = it->second;
    }
    if (cache == nullptr || storage_cache_number != storage->get_cache_number() || storage_id != storage->get_storage_id()) {
        storage_cache_number = storage->get_cache_number();
        storage_id = storage->get_storage_id();
        cache = storage->get_node_ptr(id);
        if (cache == nullptr) return false;
    }
    return true;
}

node* node_ref::get_node() const {
    if (exists()) return cache;
    return nullptr;
}

node* node_ref::get_node() {
    if (exists()) return cache;
    return nullptr;
}

