#include "event.h"

using namespace engine;

event_manager::~event_manager()  {
    for (const auto& [h, disp] : dispatchers) {
        delete disp;
    }
}

void event_manager::dispatch_queued_events() {
    for (auto& [h, disp] : dispatchers) {
        disp->dispatch_all_events();
    }
    for (auto& [h, disp] : dispatchers) {
        disp->finish_and_clear_all_events();
    }
}