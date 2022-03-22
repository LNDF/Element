#include "backend.h"
#include <stdexcept>

Backend* currentBackendInstance = nullptr;

Backend::Backend() {
    if (currentBackendInstance != nullptr) {
        throw std::runtime_error("Backend already created.");
    }
}

Backend::~Backend() {
    currentBackendInstance = nullptr;
}