#pragma once

#include <resource/loader.h>
#include <utils/uuid.h>
#include <utils/thread_pool.h>

namespace element {
    namespace resource {
        
        template<typename T, void(*F)(const uuid&, const T&)>
        void async_load(const uuid& id) {
            thread_pool::run_task([=]() {
                std::optional<T> t = load_resource<T>(id);
                if (t != std::nullopt) {
                    F(id, *t);
                }
            });
        }

    } //namespace resource
} //namespace element