#include "init.h"

#include <core/application.h>
#include <event/event.h>

using namespace element;

void init::setup_engine(const startup_settings& settings) {
    application::configure();
    //TODO: configure engine here: physx, other stuff...
}

void init::cleanup() {
    event_manager::cleanup();
    //TODO: cleanup used memory and resources: physx, other stuff...
}