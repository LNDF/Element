#pragma once

#include <utils/packed_map.h>
#include <utils/uuid.h>
#include <scenegraph/node_manager.h>
#include <scenegraph/node_ref.h>
#include <cinttypes>
#include <typeindex>
#include <type_traits>

namespace element {
    namespace scenegraph {
        class scene;
        class node;

        class node_storage_base {
            private:
                std::uint32_t storage_id;

                static std::uint32_t next_storage_id;
            protected:
                std::uint32_t cache_number = 0;
                scenegraph::scene* owner_scene = nullptr;

                void setup_node(node& n);
                void cleanup_node(node& n);
                void set_owner_scene_node(node& n);
            public:
                node_storage_base();
                node_storage_base(scenegraph::scene* owner_scene);
                virtual ~node_storage_base() = default;

                inline std::uint32_t get_cache_number() const {return cache_number;}
                inline std::uint32_t get_storage_id() const {return storage_id;}
                inline scenegraph::scene* get_owner_scene() const {return owner_scene;}

                virtual void delete_node(const uuid& id) = 0;
                virtual void emplace_node(const uuid& id, const std::string& name, const node_ref& parent) = 0;
                virtual void emplace_node(const uuid& id, std::string&& name, const node_ref& parent) = 0;
                virtual scenegraph::node* get_node_ptr(const uuid& id) = 0;
                virtual std::type_index get_type_index() = 0;
                virtual void init_scene(scenegraph::scene* owner_scene) = 0;

        };

        template<typename T, typename = std::enable_if<std::is_base_of_v<scenegraph::node, T>>>
        class node_storage : public node_storage_base {
            private:
                packed_map<uuid, T> storage;
            public:
                node_storage(scenegraph::scene* owner_scene) {}

                ~node_storage() final override {
                    if (owner_scene == nullptr) return;
                    for (auto& [id, n] : storage) {
                        cleanup_node(n);
                    }
                    for (auto& [id, n] : storage) {
                        scenegraph::delete_node_storage_mapping(id);
                    }
                }

                void delete_node(const uuid& id) final override {
                    auto it = storage.find(id);
                    if (it == storage.end()) return;
                    if (owner_scene != nullptr) {
                        cleanup_node(it->second);
                        scenegraph::delete_node_storage_mapping(id);
                    }
                    storage.erase(it);
                    this->cache_number++;
                }

                void emplace_node(const uuid& id, const std::string& name, const node_ref& parent) final override {
                    if (owner_scene != nullptr) {
                        scenegraph::update_node_storage_mapping(id, this);
                    }
                    auto it = storage.try_emplace(id, id, name, parent).first;
                    this->cache_number++;
                    if (owner_scene != nullptr) {
                        setup_node(it->second);
                    }
                }

                void emplace_node(const uuid& id, std::string&& name, const node_ref& parent) final override {
                    if (owner_scene != nullptr) {
                        scenegraph::update_node_storage_mapping(id, this);
                    }
                    auto it = storage.try_emplace(id, id, std::move(name), parent).first;
                    this->cache_number++;
                    if (owner_scene != nullptr) {
                        setup_node(it->second);
                    }
                }

                T& at(const uuid& id) {
                    return storage.at(at);
                }

                scenegraph::node* get_node_ptr(const uuid& id) final override {
                    auto it = storage.find(id);
                    if (it == storage.end()) return nullptr;
                    return (scenegraph::node*) std::addressof(*it);
                }

                std::type_index get_type_index() final override {
                    return std::type_index(typeid(T));
                }

                void init_scene(scenegraph::scene* owner_scene) final override {
                    this->owner_scene = owner_scene;
                    for (auto& [id, n] : storage) {
                        scenegraph::update_node_storage_mapping(id, this);
                    }
                    for (auto& [id, n] : storage) {
                        set_owner_scene_node(n);
                    }
                    for (auto& [id, n] : storage) {
                        setup_node(n);
                    }
                }

                inline const packed_map<uuid, T>& get_storage() const {return storage;}

        };

        template<typename T, typename = std::enable_if<std::is_base_of_v<scenegraph::node, T>>>
        node_storage_base* node_storage_factory(scene* owner_scene) {
            return new node_storage<T>(owner_scene);
        }
    } // namespace scenegraph
    
} // namespace element
