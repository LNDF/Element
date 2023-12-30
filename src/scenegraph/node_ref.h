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
                const node* get_node() const;
                node* get_node();

                inline const node* operator->() const noexcept {
                    return get_node();
                }

                inline const node& operator*() const noexcept {
                    return *get_node();
                }

                inline node* operator->() noexcept {
                    return get_node();
                }

                inline node& operator*() noexcept {
                    return *get_node();
                }

                inline bool operator==(const node_ref& other) const noexcept {
                    return id == other.id;
                }

                inline bool operator==(const std::nullptr_t other) const noexcept {
                    return !exists();
                }

                inline const uuid& get_id() const {return id;}
                inline void set_id(const uuid& id) {
                    this->id = id;
                    this->cache = nullptr;
                    this->storage = nullptr;
                }
        };

        template<typename T>
        class node_ref_derived {
            private:
                node_ref ref;
            public:
                node_ref_derived() {}
                node_ref_derived(const uuid& id) : ref(id) {}
                node_ref_derived(T* node) : ref(node) {}
                node_ref_derived(std::nullptr_t) : ref(nullptr) {}
                node_ref_derived(const node_ref& ref) : ref(ref) {}
                inline bool exists() const {return ref.exists();}
                inline const T* get_node() const {return static_cast<const T*>(ref.get_node());}
                inline T* get_node() {return static_cast<T*>(ref.get_node());}
                
                inline const T* operator->() const noexcept {return static_cast<const T*>(ref.operator->());}
                inline const T& operator*() const noexcept {return static_cast<const T&>(ref.operator*());}
                inline T* operator->() noexcept {return static_cast<T*>(ref.operator->());}
                inline T& operator*() noexcept {return static_cast<T&>(ref.operator*());}

                inline bool operator==(const node_ref_derived& other) const noexcept {return ref.operator==(other.ref);}
                inline bool operator==(const std::nullptr_t other) const noexcept {return ref.operator==(nullptr);}

                inline const uuid& get_id() const {return ref.get_id();}
                inline void set_id(const uuid& id) {ref.set_id(id);}
        };
    } // namespace scenegraph
} // namespace element
