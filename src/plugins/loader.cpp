#include "loader.h"

#include <core/log.h>
#include <element/defs.h>
#include <element/types/module.h>

#define ELM_PLUGIN_ENTRY_SYMBOL "__elm_plugin_setup"
#define ELM_PLUGIN_CLEANUP_SYMBOL "__elm_plugin_cleanup"

#ifdef ELM_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(ELM_PLATFORM_LINUX)
#include <dlfcn.h>
#endif

ELM_FUNCTION(plugin_setup, void, const element_modules_t* modules);
ELM_FUNCTION(plugin_cleanup, void);

using namespace element;

static std::unordered_map<plugins::handle, plugins::plugin_info> loaded_plugins;

const plugins::plugin_info* plugins::load_plugin(const std::filesystem::path& path) {
    std::filesystem::path full_path = std::filesystem::absolute(path);
    ELM_INFO("Loading plugin: {}", full_path.string());
#ifdef ELM_PLATFORM_WINDOWS
    handle plugin = LoadLibraryW(full_path.c_str());
#elif defined(ELM_PLATFORM_LINUX)
    handle plugin = dlopen(full_path.c_str(), RTLD_NOW);
#endif
    if (!plugin) {
        ELM_ERROR("Failed to load plugin: {}", full_path.string());
        return nullptr;
    }
    plugin_info info;
    info.path = full_path;
    element_plugin_setup_t setup = (element_plugin_setup_t) get_plugin_symbol(plugin, ELM_PLUGIN_ENTRY_SYMBOL);
    if (setup) {
        info.has_element_symbols = true;
        //TODO: build modules list
        setup(nullptr);
    } else {
        info.has_element_symbols = false;
    }
    return &loaded_plugins.emplace(plugin, std::move(info)).first->second;
}

void plugins::unload_plugin(handle plugin) {
    auto it = loaded_plugins.find(plugin);
    if (it == loaded_plugins.end()) {
        ELM_ERROR("Failed to unload plugin: plugin not found");
        return;
    }
    auto& info = it->second;
    ELM_INFO("Unloading plugin: {}", info.path.string());
    if (info.has_element_symbols) {
        //TODO: Use correct typedef for function pointer
        element_plugin_cleanup_t cleanup = (element_plugin_cleanup_t) get_plugin_symbol(plugin, ELM_PLUGIN_CLEANUP_SYMBOL);
        if (cleanup) {
            cleanup();
        } else {
            ELM_WARN("Failed to find plugin cleanup function in plugin: {}", info.path.string());
        }
    }
#ifdef ELM_PLATFORM_WINDOWS
    FreeLibrary(plugin);
#elif defined(ELM_PLATFORM_LINUX)
    dlclose(plugin);
#endif
    loaded_plugins.erase(it);
}

void plugins::unload_all_plugins() {
    for (auto& [plugin, info] : loaded_plugins) {
        unload_plugin(plugin);
    }
}

void* plugins::get_plugin_symbol(handle plugin, const char* symbol) {
#ifdef ELM_PLATFORM_WINDOWS
    return GetProcAddress(plugin, symbol);
#elif defined(ELM_PLATFORM_LINUX)
    return dlsym(plugin, symbol);
#endif
}

const std::unordered_map<plugins::handle, plugins::plugin_info>& plugins::get_loaded_plugins() {
    return loaded_plugins;
}