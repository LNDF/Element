#pragma once

#include <utils/uuid.h>
#include <cinttypes>

namespace element {
    namespace scenegraph {
        class node;
        class node_storage_base;

        class node_ref {
            private:
                uuid id;
                mutable node* cache = nullptr;
                mutable node_storage_base* storage = nullptr;
                mutable std::uint32_t storage_id;
                mutable std::uint32_t storage_cache_number;
                mutable std::uint32_t manager_cache_number;
            public:
                node_ref();
                node_ref(const uuid& id);
                node_ref(node* node);
                node_ref(std::nullptr_t);
                bool exists() const;
                node* get_node() const;
                node* get_node();

                inline node* operator->() const noexcept {
                    return get_node();
                }

                inline node& operator*() const noexcept {
                    return *get_node();
                }

                inline bool operator==(const node_ref& other) const noexcept {
                    return id == other.id;
                }

                inline bool operator==(const std::nullptr_t other) const noexcept {
                    return exists();
                }

                inline const uuid& get_id() const {return id;}
                inline void set_id(const uuid& id) {
                    this->id = id;
                    this->cache = nullptr;
                    this->storage = nullptr;
                }
        };
    } // namespace scenegraph
} // namespace element
