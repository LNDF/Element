#include "engine.h"

#include <core/log.h>
#include <event/event.h>
#include <render/display.h>

using namespace element;

bool engine::closed = false;
engine_settings engine::settings;

bool engine::close_event_listener(const events::close&) {
    closed = true;
    return true;
}

void engine::init() {
    #ifdef ELM_ENABLE_LOGGING
    log::init_log();
    ELM_SET_LOG_LEVEL(ELM_LOG_LEVEL_TRACE); //TEMP
    #endif
    ELM_INFO("Element engine version {0} starting...", ELM_VERSION);
    ELM_INFO("Application {0} version {1}", settings.app_name, settings.app_version);
    ELM_INFO("Configuring application...");
    event_manager::register_default_listener<events::close>(close_event_listener);
    ELM_INFO("Configuration done");
}

void engine::setup_engine(const engine_settings& settings) {
    engine::settings = settings;
    init();
}

void engine::setup_engine(engine_settings&& settings) {
    engine::settings = std::move(settings);
    init();
}

void engine::cleanup_engine() {
    ELM_INFO("Application will close soon. Cleanning up...");
    cleanup_displays();
    event_manager::cleanup();
}

void engine::start() {
    ELM_INFO("Starting application...");
}

void engine::tick() {
    event_manager::send_event<events::update>({0}); //TODO: correct delta time
    event_manager::dispatch_queued_events();
}

void engine::stop() {
    ELM_INFO("Stopping application...");
}

void engine::execute() {
    ELM_DEBUG("Applilcation execute called");
    start();
    while (!should_close()) tick();
    stop();
}