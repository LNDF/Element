#include "event.h"

using namespace element;

packed_map<std::type_index, __detail::__event_manager_dispatcher_base*> __detail::__event_manager_dispatchers;

void event_manager::cleanup()  {
    for (const auto& [h, disp] : __detail::__event_manager_dispatchers) {
        delete disp;
    }
    __detail::__event_manager_dispatchers.clear();
}

void event_manager::dispatch_queued_events() {
    for (auto& [h, disp] : __detail::__event_manager_dispatchers) {
        disp->dispatch_all_events();
    }
    for (auto& [h, disp] : __detail::__event_manager_dispatchers) {
        disp->finish_and_clear_all_events();
    }
}