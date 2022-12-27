#pragma once

#include <core/log.h>
#include <event/event.h>
#include <event/core_events.h>

namespace element {
    struct startup_settings {
        //TODO: here will appear startup settings
        
    };

    class application {
        private:
            static bool closed;

            static bool close_event_listener(const events::close&);
        public:
            static void setup_engine(const startup_settings& settings);
            static void cleanup_engine();
            static void start();
    };
} // namespace element
