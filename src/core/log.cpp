#include "log.h"

using namespace element;

#ifdef ELM_ENABLE_LOGGING

std::shared_ptr<spdlog::logger> log::console_log;
std::shared_ptr<spdlog::logger> log::error_log;

void log::init_log() {
    console_log = spdlog::stdout_color_mt("console");
    error_log = spdlog::stderr_color_mt("error");
    console_log->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %!@%s:%# %l]: %v%$");
    error_log->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %!@%s:%# %l]: %v%$");
}

#endif