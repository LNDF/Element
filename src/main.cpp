#include <iostream>

#include <event/event.h>

using namespace element;

bool fl(const int& i) {
    std::cout << "FL: " << i << std::endl;
    return true;
}

bool dl(const int& i) {
    std::cout << "DL: " << i << std::endl;
    return true;
}

bool l1(const int& i) {
    std::cout << "L1: " << i << std::endl;
    return true;
}

bool l2(const int& i) {
    std::cout << "L2: " << i << std::endl;
    return true;
}

void done(const int& i) {
    std::cout << "Done " << i << std::endl;
}

void cancel(const int& i) {
    std::cout << "Cancel " << i << std::endl;
}

int main() {
    event_manager mgr;
    mgr.register_default_listener<int>(dl);
    mgr.register_first_listener<int>(fl);
    mgr.register_listener<int>(l2);
    mgr.register_listener<int>(l1);
    mgr.post_event<int>(12, done, cancel);
    mgr.post_event<int>(15, done, cancel);
    mgr.dispatch_queued_events();
    mgr.dispatch_queued_events();
    return 0;
}