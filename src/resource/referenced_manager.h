#pragma once

#include <utils/uuid.h>
#include <unordered_map>
#include <utility>

namespace element {
    namespace resource {
        
        template<typename T, T*(*C)(const uuid&), void(*D)(const uuid&), T*(*G)(const uuid&)>
        class referenced_resource_manager {
            private:
                static std::unordered_map<uuid, std::uint32_t> ref_map;
            public:
                static std::pair<T*, std::uint32_t> get_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) {
                        return std::make_pair(nullptr, 0);
                    }
                    return std::make_pair(G(id), it->second);
                }

                const T* claim_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    if (it == ref_map.end()) {
                        it = ref_map.insert_or_assign(id, 0)).first;
                    }
                    T* res;
                    if (it->second == 0) {
                        res = C(id);
                    } else {
                        res = G(id);
                    }
                    return std::make_pair(res, ++it->second);
                }

                const void release_resource(const uuid& id) {
                    auto it = ref_map.find(id);
                    it (it == ref_map.end()) return;
                    if (--it->second == 0) D(id);
                }
        };

    } //namespace resource
} // namespace element

template<typename T, T*(*C)(const element::uuid&), void(*D)(const element::uuid&), T*(*G)(const element::uuid&)>
std::unordered_map<element::uuid, std::uint32_t> element::resource::referenced_resource_manager<T, C, D, G>::ref_map;