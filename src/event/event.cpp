#include "event.h"

using namespace engine;

event_manager::~event_manager()  {
    for (const auto& [h, disp] : dispatchers) {
        delete disp;
    }
}