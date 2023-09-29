#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>
#include <functional>

namespace element {
    namespace scenegraph {
        struct node_type_info;
    } // namespace scenegraph
    

    namespace __detail {
        std::unordered_map<std::type_index, scenegraph::node_type_info>& __scenegraph_get_node_type_map();
    } // namespace __detail
    

    namespace scenegraph {
        class scene;
        class node_storage_base;
        
        struct node_type_info {
            using storage_factory_type = std::function<node_storage_base*(scene*)>;

            std::string name;
            storage_factory_type storage_factory;

            node_type_info(const std::string& name, storage_factory_type storage_factory);
            node_type_info(std::string&& name, storage_factory_type storage_factory);
        };

        const node_type_info& get_node_type_info(std::type_index type);

    } // namespace scenegraph
} // namespace element
