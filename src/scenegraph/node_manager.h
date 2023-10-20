#pragma once

#include <unordered_map>
#include <cinttypes>
#include <utils/uuid.h>

namespace element {
    namespace scenegraph {
        class node_storage_base;
    } // namespace scenegraph

    namespace scenegraph {
        node_storage_base* get_node_storage_mapping(const uuid& id);
        void update_node_storage_mapping(const uuid& id, node_storage_base* storage);
        void delete_node_storage_mapping(const uuid& id);

        std::uint32_t get_node_manager_cache_number();
    } // namespace scenegraph
    
} // namespace element
