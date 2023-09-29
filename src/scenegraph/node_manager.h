#pragma once

#include <unordered_map>
#include <cinttypes>
#include <utils/uuid.h>

namespace element {
    namespace scenegraph {
        class node_storage_base;
    } // namespace scenegraph

    namespace __detail {
        extern std::unordered_map<uuid, scenegraph::node_storage_base*> __scenegraph_node_storage_map;
        extern std::uint32_t __scenegraph_node_manager_cache_number;
    } // namespace __detail

    namespace scenegraph {

        uuid get_new_node_id();

        void update_node_storage_mapping(const uuid& id, node_storage_base* storage);
        void delete_node_storage_mapping(const uuid& id);

        inline std::uint32_t get_node_manager_cache_number() {return __detail::__scenegraph_node_manager_cache_number;}
    } // namespace scenegraph
    
} // namespace element
