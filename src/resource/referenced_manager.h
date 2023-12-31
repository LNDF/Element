#pragma once

#include <utils/uuid.h>
#include <unordered_map>
#include <utility>

namespace element {
    namespace resource {
        
        template<typename T, void(*C)(const uuid&), void(*D)(const uuid&), T*(*G)(const uuid&)>
        class referenced_resource_manager {
            private:
                static std::unordered_map<uuid, std::uint32_t> ref_map;
            public:
                static std::uint32_t get_resource_references(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) {
                        return 0;
                    }
                    return it->second;
                }

                static std::pair<T*, std::uint32_t> get_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) {
                        return std::make_pair(nullptr, 0);
                    }
                    return std::make_pair(G(id), it->second);
                }

                static void claim_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) {
                        it = ref_map.insert_or_assign(id, 0).first;
                    }
                    if (it->second++ == 0) {
                        C(id);
                    }
                }

                static void release_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) return;
                    if (--it->second == 0) {
                        D(id);
                        ref_map.erase(it);
                    }
                }

                static void destroy_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) return;
                    D(id);
                    ref_map.erase(it);
                }

                static void destroy_all_resources() {
                    for (auto& [id, count] : ref_map) {
                        D(id);
                    }
                    ref_map.clear();
                }

                static void reload_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) return;
                    D(id);
                    C(id);
                }

                static void reload_all_resources() {
                    for (auto& [id, count] : ref_map) {
                        D(id);
                        C(id);
                    }
                }
        };

    } //namespace resource
} // namespace element

template<typename T, void(*C)(const element::uuid&), void(*D)(const element::uuid&), T*(*G)(const element::uuid&)>
std::unordered_map<element::uuid, std::uint32_t> element::resource::referenced_resource_manager<T, C, D, G>::ref_map;