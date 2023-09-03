#include "engine.h"

#include <core/log.h>
#include <core/fs.h>
#include <event/event.h>
#include <graphics/vulkan.h>
#include <utils/uuid.h>

using namespace element;

bool engine::closed = false;
engine_settings engine::settings;

bool engine::close_event_listener(const events::close&) {
    closed = true;
    return true;
}

void engine::setup_engine() {
    ELM_INFO("Element engine version {0} starting...", ELM_VERSION);
    ELM_INFO("Application {0} version {1}", settings.app_name, settings.app_version);
    ELM_INFO("Configuring application...");
    uuid::reseed_generator();
    vulkan::init_instance();
    fs::load_resources();
    event_manager::register_default_listener<events::close>(close_event_listener);
    ELM_INFO("Configuration done");
}

void engine::cleanup_engine() {
    ELM_INFO("Application will close soon. Cleanning up...");
    event_manager::cleanup();
    vulkan::cleanup();
}

void engine::start() {
    ELM_INFO("Starting application...");
}

void engine::tick() {
    event_manager::send_event<events::update>({0}); //TODO: correct delta time
    event_manager::dispatch_queued_events();
}

void engine::stop() {
    
}

void engine::execute() {
    ELM_DEBUG("Applilcation execute called");
    start();
    while (!should_close()) tick();
    stop();
}