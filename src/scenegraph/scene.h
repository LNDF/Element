#pragma once

#include <scenegraph/node_ref.h>
#include <scenegraph/node_storage.h>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <string>

namespace element {
    namespace scenegraph {

        class scene {
            private:
                uuid id;
                std::unordered_map<std::type_index, std::unique_ptr<node_storage_base>> node_storages;
                node_ref root_node = nullptr;
            public:
                scene();
                ~scene();
                scene(const scene& other) = delete;
                scene(scene&&) = default;
                scene& operator=(const scene& other) = delete;
                scene& operator=(scene& other) = default;

                node_storage_base* get_storage(std::type_index type);

                void init_scene(const uuid& id);

                inline const uuid& get_id() const {return id;}
                inline const node_ref& get_root_node() const {return root_node;}
                inline const std::unordered_map<std::type_index, std::unique_ptr<node_storage_base>>& get_node_storages() const {return node_storages;}
                inline void set_root_node(const node_ref& root_node) {if (id.is_null()) this->root_node = root_node;}
                inline void set_node_storages(std::unordered_map<std::type_index, std::unique_ptr<node_storage_base>>&& storages) {
                    if (id.is_null()) this->node_storages = std::move(storages);
                }
        };

    } // namespace scenegraph
} // namespace element
