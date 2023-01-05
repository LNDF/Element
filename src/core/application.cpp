#include "application.h"

#include <core/log.h>
#include <event/event.h>
#include <ui/ui.h>

using namespace element;

bool application::closed = false;
application_settings application::settings;

bool application::close_event_listener(const events::close&) {
    closed = true;
    return true;
}

void application::init() {
    #ifdef ELM_ENABLE_LOGGING
    log::init_log();
    ELM_SET_LOG_LEVEL(ELM_LOG_LEVEL_TRACE); //TEMP
    #endif
    ELM_INFO("Element engine version {0} starting...", ELM_VERSION);
    ELM_INFO("Application {0} version {1}", settings.app_name, settings.app_version);
    ELM_INFO("Configuring application...");
    event_manager::register_default_listener<events::close>(close_event_listener);
    ui::init_backend();
    ELM_INFO("Configuration done");
}

void application::setup_engine(const application_settings& settings) {
    application::settings = settings;
    init();
}

void application::setup_engine(application_settings&& settings) {
    application::settings = std::move(settings);
    init();
}

void application::cleanup_engine() {
    ELM_INFO("Application will close soon. Cleanning up...");
    ui::cleanup_backend();
    event_manager::cleanup();
}

void application::start() {
    ELM_INFO("Application entering main loop now");
    ui::start_ui();
    while (!closed) {
        event_manager::send_event<events::update>({0}); //TODO: correct delta time
        event_manager::dispatch_queued_events();
    }
    ui::stop_ui();
}