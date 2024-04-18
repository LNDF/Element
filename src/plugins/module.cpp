#include "module.h"

using namespace element;

static std::unordered_map<std::string, void*>& get_modules_map() {
    static std::unordered_map<std::string, void*> modules;
    return modules;
}

void plugins::register_module(const std::string& name, void* module) {
    get_modules_map()[name] = module;
}

void* plugins::get_module(const std::string& name) {
    auto it = get_modules_map().find(name);
    if (it == get_modules_map().end()) return nullptr;
    return it->second;
}

void plugins::unregister_module(const std::string& name) {
    get_modules_map().erase(name);
}

const std::unordered_map<std::string, void*>& plugins::get_modules() {
    return get_modules_map();
}