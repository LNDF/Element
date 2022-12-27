#include "application.h"

using namespace element;

bool application::closed = false;

bool application::close_event_listener(const events::close&) {
    closed = true;
    return true;
}

void application::setup_engine(const startup_settings& settings) {
    #ifdef ELM_ENABLE_LOGGING
    log::init_log();
    #endif
    event_manager::register_default_listener<events::close>(close_event_listener);
}

void application::cleanup_engine() {
    event_manager::cleanup();
}

void application::start() {
    while (!closed) {
        event_manager::send_event<events::update>({0}); //TODO: correct delta time
        event_manager::dispatch_queued_events();
    }
}