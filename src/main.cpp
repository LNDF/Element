#include <iostream>

#include <core/log.h>

int main() {
    #ifdef ELM_ENABLE_LOGGING
    element::log::init_log();
    #endif
    ELM_ERROR("THIS IS A TEST");
    return 0;
}