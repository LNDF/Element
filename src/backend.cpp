#include "backend.h"
#include <stdexcept>

Backend* currentBackendInstance = nullptr;

std::string dummyBackendName;

Backend::Backend() {
    if (currentBackendInstance != nullptr) {
        throw std::runtime_error("Backend already created.");
    }
}

Backend::~Backend() {
    currentBackendInstance = nullptr;
}

void Backend::mainLoop() {};
void Backend::close() {};
void Backend::notClose() {};
void Backend::setWidth(u32 width) {};
void Backend::setHeight(u32 height) {};
void Backend::setName(const std::string& name) {};
void Backend::setMainLoopCallback(MainLoopCallback mainLoopCallback) {};
const std::string& Backend::getName() const { return dummyBackendName; };
u32 Backend::getWidth() const { return 0; };
u32 Backend::getHeight() const { return 0; };
MainLoopCallback Backend::getMainLoopCallback() const { return nullptr; };

//states
void Backend::setSwapInterval(int interval) {};
void Backend::setViewport(u32 width, u32 height) {};

//utils
void Backend::
clear() {};