#include "log.h"

using namespace element;

#ifdef ELM_ENABLE_LOGGING

std::shared_ptr<spdlog::logger> log::console_log;
std::shared_ptr<spdlog::logger> log::error_log;

void log::setup() {
    console_log = spdlog::stdout_color_mt("console");
    error_log = spdlog::stderr_color_mt("error");
    console_log->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %!@%s:%# %l]: %v%$");
    error_log->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e %!@%s:%# %l]: %v%$");
}

std::shared_ptr<spdlog::logger> log::get_logger(log_level level) {
    switch (level) {
        case ELM_LOG_LEVEL_ERROR:
        case ELM_LOG_LEVEL_CRITICAL:
        case ELM_LOG_LEVEL_DISABLE:
            return error_log;
        case ELM_LOG_LEVEL_TRACE:
        case ELM_LOG_LEVEL_INFO:
        case ELM_LOG_LEVEL_DEBUG:
        case ELM_LOG_LEVEL_WARN:
        default:
            return console_log;
    }
}

#endif