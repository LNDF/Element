#pragma once

#include <event/core_events.h>
#include <string>

namespace element {
    struct application_settings {
        std::string app_name;
        std::string app_version;
    };

    class application {
        private:
            static bool closed;
            static application_settings settings;

            static void init();

            static bool close_event_listener(const events::close&);
        public:
            static void setup_engine(const application_settings& settings);
            static void setup_engine(application_settings&& settings);
            static void cleanup_engine();
            static void start();
    };
} // namespace element
