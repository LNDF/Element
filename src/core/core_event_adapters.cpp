#include <core/core_events.h>
#include <event/event.h>
#include <plugins/event_provider.h>
#include <element/events.h>

using namespace element;

static bool close_event_adapter(events::close& e, void* function) {
    return ((uint32_t(*)())function)();
}

ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_CORE_CLOSE, events::close, close_event_adapter, event_callback_priority::medium)