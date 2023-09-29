#pragma once

#include <scenegraph/node_ref.h>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <string>

namespace element {
    namespace scenegraph {
        class node_storage_base;

        class scene {
            private:
                uuid id;
                std::string name;
                std::unordered_map<std::type_index, node_storage_base*> node_storages;
                node_ref root_node = nullptr;
            public:
                explicit scene(const std::string& name);
                explicit scene(std::string&& name);
                ~scene();

                node_storage_base* get_storage(std::type_index type);

                void init_scene(const uuid& id);

                inline const uuid& get_id() const {return id;}
                inline const std::string& get_name() const {return name;}
                inline const node_ref& get_root_node() const {return root_node;}
                inline const std::unordered_map<std::type_index, node_storage_base*>& get_node_storages() const {return node_storages;}
                inline void set_name(const std::string& name) {this->name = name;}
                inline void set_name(std::string&& name) {this->name = std::move(name);}
        };

    } // namespace scenegraph
} // namespace element
