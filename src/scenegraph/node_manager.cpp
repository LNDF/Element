#include "node_manager.h"

using namespace element;

std::unordered_map<uuid, scenegraph::node_storage_base*> __detail::__scenegraph_node_storage_map;
std::uint32_t __detail::__scenegraph_node_manager_cache_number = 0;

uuid scenegraph::get_new_node_id() {
    uuid id;
    auto it = __detail::__scenegraph_node_storage_map.find(id);
    while (it != __detail::__scenegraph_node_storage_map.end()) {
        id.regenerate();
        it = __detail::__scenegraph_node_storage_map.find(id);
    }
    return id;
}

void scenegraph::update_node_storage_mapping(const uuid& id, node_storage_base* storage) {
    if (!__detail::__scenegraph_node_storage_map.insert_or_assign(id, storage).second) {
        __detail::__scenegraph_node_manager_cache_number++;
    }
}

void scenegraph::delete_node_storage_mapping(const uuid& id) {
    auto it = __detail::__scenegraph_node_storage_map.find(id);
    if (it != __detail::__scenegraph_node_storage_map.end()) {
        __detail::__scenegraph_node_storage_map.erase(it);
        __detail::__scenegraph_node_manager_cache_number++;
    }
}