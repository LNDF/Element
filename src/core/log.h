#pragma once

#ifdef ELM_ENABLE_LOGGING

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#endif

namespace element {
    
    #ifdef ELM_ENABLE_LOGGING

    class log {
        public:
            static std::shared_ptr<spdlog::logger> console_log;
            static std::shared_ptr<spdlog::logger> error_log;
            static void init_log();
            
    };

    #endif

} // namespace element

#ifdef ELM_ENABLE_LOGGING
#define ELM_TRACE(...) SPDLOG_LOGGER_CALL(element::log::console_log, spdlog::level::trace, __VA_ARGS__)
#define ELM_DEBUG(...) SPDLOG_LOGGER_CALL(element::log::console_log, spdlog::level::debug   , __VA_ARGS__)
#define ELM_INFO(...) SPDLOG_LOGGER_CALL(element::log::console_log, spdlog::level::info, __VA_ARGS__)
#define ELM_WARN(...) SPDLOG_LOGGER_CALL(element::log::console_log, spdlog::level::warn, __VA_ARGS__)
#define ELM_ERROR(...) SPDLOG_LOGGER_CALL(element::log::error_log, spdlog::level::err, __VA_ARGS__)
#define ELM_CRITICAL(...) SPDLOG_LOGGER_CALL(element::log::error_log, spdlog::level::critical, __VA_ARGS__)
#define ELM_SET_LOG_LEVEL(x) {element::log::console_log->set_level(x); element::log::error_log->set_level(x);}
#define ELM_LOG_LEVEL_TRACE spdlog::level::trace
#define ELM_LOG_LEVEL_DEBUG spdlog::level::debug
#define ELM_LOG_LEVEL_INFO spdlog::level::info
#define ELM_LOG_LEVEL_WARN spdlog::level::warn
#define ELM_LOG_LEVEL_ERROR spdlog::level::err
#define ELM_LOG_LEVEL_CRITICAL spdlog::level::critical
#define ELM_LOG_LEVEL_DISABLE spdlog::level::off
#else
#define ELM_TRACE(...)
#define ELM_DEBUG(...)
#define ELM_INFO(...)
#define ELM_WARN(...)
#define ELM_ERROR(...)
#define ELM_CRITICAL(...)
#define ELM_SET_LOG_LEVEL(x)
#define ELM_LOG_LEVEL_TRACE
#define ELM_LOG_LEVEL_DEBUG
#define ELM_LOG_LEVEL_INFO
#define ELM_LOG_LEVEL_WARN
#define ELM_LOG_LEVEL_ERROR
#define ELM_LOG_LEVEL_CRITICAL
#define ELM_LOG_LEVEL_DISABLE
#endif