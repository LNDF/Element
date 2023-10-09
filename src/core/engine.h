#pragma once

#include <core/core_events.h>
#include <string>

namespace element {
    
    namespace __detail {

        extern bool __engine_closed;

        void __engine_init();
        bool __engine_close_event_listener(events::close&);
        
    } // namespace __detail
    

    namespace engine {
        struct settings_t {
            std::string app_name;
            std::string app_version;
        };

        extern settings_t settings;

        void setup();
        void start();
        void tick();
        void stop();
        void cleanup();
        void execute();
        inline bool should_close() {return __detail::__engine_closed;}
    } // namespace engine
    
} // namespace element
