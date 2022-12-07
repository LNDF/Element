#include "event.h"

using namespace element;

packed_map<std::type_index, event_dispatcher_base*> event_manager::dispatchers;

void event_manager::cleanup()  {
    for (const auto& [h, disp] : dispatchers) {
        delete disp;
    }
    dispatchers.clear();
}

void event_manager::dispatch_queued_events() {
    for (auto& [h, disp] : dispatchers) {
        disp->dispatch_all_events();
    }
    for (auto& [h, disp] : dispatchers) {
        disp->finish_and_clear_all_events();
    }
}