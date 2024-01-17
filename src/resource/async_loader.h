#pragma once

#include <resource/loader.h>
#include <resource/async_loader_events.h>
#include <event/event.h>
#include <utils/uuid.h>
#include <utils/thread_pool.h>

namespace element {
    namespace resource {
        
        template<typename T, void(*F)(const uuid&, const T&)>
        void async_load(const uuid& id) {
            thread_pool::run_task([=]() {
                std::optional<T> t = load_resource<T>(id);
                if (t != std::nullopt) {
                    events::resource_end_loading end;
                    end.id = id;
                    F(id, *t);
                    event_manager::send_event(end);
                }
            });
            events::resource_start_loading start;
            start.id = id;
            event_manager::send_event(start);
        }

    } //namespace resource
} //namespace element