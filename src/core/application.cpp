#include "application.h"

using namespace element;

bool application::closed = false;

void application::configure() {
    event_manager::register_default_listener<events::close>(close_event_listener);
}

bool application::close_event_listener(const events::close&) {
    closed = true;
    return true;
}

void application::start() {
    while (!closed) {
        event_manager::send_event<events::update>({0}); //TODO: correct delta time
        event_manager::dispatch_queued_events();
    }
}