#pragma once

#include <event/event.h>
#include <event/core_events.h>

namespace element {
    class application {
        private:
            static bool closed;

            static bool close_event_listener(const events::close&);
        public:
            static void configure();

            static void start();
    };
} // namespace element
