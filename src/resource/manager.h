#pragma once

#include <resource/loader.h>
#include <utils/uuid.h>
#include <unordered_map>

namespace element {
    namespace resource {
        
        template<typename T>
        class manager {
            private:
                static std::unordered_map<uuid, T> loaded;
            public:
                static ELM_MAKE_RESOURCE_LOADER(load, T)

                static T* get(const uuid& id) {
                    auto it = loaded.find(id);
                    if (it == loaded.end()) {
                        auto res = load(id);
                        if (res == std::nullopt) return nullptr;
                        it = loaded.insert_or_assign(id, std::move(res.value())).first;
                    }
                    return it->second;
                }

                static void destroy(const uuid& id) {
                    loaded.erase(id);
                }

                static void destroy_all() {
                    loaded.clear();
                }

                static inline std::unordered_map<uuid, T>& get_map() {return loaded;}
        };


    } // namespace resource
} // namespace element

template<typename T>
std::unordered_map<element::uuid, T> element::resource::manager<T>::loaded;