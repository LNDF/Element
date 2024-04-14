#include <display/display_events.h>
#include <event/event.h>
#include <plugins/event_provider.h>
#include <element/events.h>

using namespace element;

static bool mouse_down_event_adapter(events::mouse_down& e, void* function) {
    return ((uint32_t(*)(std::uint32_t))function)(e.button);
}

static bool mouse_up_event_adapter(events::mouse_up& e, void* function) {
    return ((uint32_t(*)(std::uint32_t))function)(e.button);
}

static bool key_down_event_adapter(events::key_down& e, void* function) {
    return ((uint32_t(*)(std::uint32_t))function)(e.key);
}

static bool key_up_event_adapter(events::key_up& e, void* function) {
    return ((uint32_t(*)(std::uint32_t))function)(e.key);
}

static bool mouse_move_event_adapter(events::mouse_move& e, void* function) {
    return ((uint32_t(*)(int, int))function)(e.x, e.y);
}

static bool window_resize_event_adapter(events::window_resize& e, void* function) {
    return ((uint32_t(*)(int, int))function)(e.width, e.height);
}

ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_MOUSE_DOWN, events::mouse_down, mouse_down_event_adapter, event_callback_priority::medium)
ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_MOUSE_UP, events::mouse_up, mouse_up_event_adapter, event_callback_priority::medium)
ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_KEY_DOWN, events::key_down, key_down_event_adapter, event_callback_priority::medium)
ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_KEY_UP, events::key_up, key_up_event_adapter, event_callback_priority::medium)
ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_MOUSE_MOVE, events::mouse_move, mouse_move_event_adapter, event_callback_priority::medium)
ELM_REGISTER_EVENT_ADAPTER(ELM_EVENT_DISPLAY_WINDOW_RESIZE, events::window_resize, window_resize_event_adapter, event_callback_priority::medium)