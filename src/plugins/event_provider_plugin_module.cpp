#include <plugins/event_provider.h>
#include <plugins/module.h>
#include <element/defs.h>
#include <element/modules/event.h>

using namespace element;

ELM_BEGIN_C

static element_event_t register_event(const char* name, void* function) {
    return event_provider::register_event(name, function);
}

static void unregister_event(const char* name, element_event_t event) {
    event_provider::unregister_event(name, event);
}

static element_module_event_t event_module = {
    register_event,
    unregister_event
};

ELM_END_C

ELM_REGISTER_MODULE("element.event", &event_module)