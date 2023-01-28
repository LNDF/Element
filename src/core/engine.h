#pragma once

#include <event/core_events.h>
#include <string>

namespace element {
    enum rendering_api {
        opengl
    };
    
    struct engine_settings {
        std::string app_name;
        std::string app_version;
        rendering_api renderer;
    };

    class engine {
        private:
            static bool closed;

            static void init();

            static bool close_event_listener(const events::close&);
        public:
            static engine_settings settings;
            static void setup_engine(const engine_settings& settings);
            static void setup_engine(engine_settings&& settings);
            static void start();
            static void tick();
            static void stop();
            static void cleanup_engine();
            static void execute();
            inline static bool should_close() {return closed;}
    };
} // namespace element
