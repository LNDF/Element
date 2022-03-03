#include "delta_time.h"
#include <chrono>

namespace deltatime {
    double time = 0;
    double last_time = 0;
}

void deltatime::set() {
    double current = deltatime::now();
    deltatime::time = current - deltatime::last_time;
    deltatime::last_time = current;
}

double deltatime::now() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    std::chrono::seconds seconds = std::chrono::duration_cast< std::chrono::seconds >(time);
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(time);
    return (double) seconds.count() + ((double) (ms.count() % 1000)/1000.0);
}