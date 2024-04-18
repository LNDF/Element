#include <core/core_events.h>
#include <event/event.h>
#include <plugins/module.h>
#include <element/defs.h>
#include <element/modules/core.h>

using namespace element;

ELM_BEGIN_C

static void close_application() {
    events::close event;
    event_manager::send_event(event);
}

static element_module_core_t core_module = {
    close_application
};

ELM_END_C

ELM_REGISTER_MODULE("element.core", &core_module)