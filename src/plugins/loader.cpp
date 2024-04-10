#include "loader.h"

#include <core/log.h>

#define ELM_PLUGIN_ENTRY_POINT "__elm_plugin_setup"
#define ELM_PLUGIN_EXIT_POINT "__elm_plugin_cleanup"

#ifdef ELM_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(ELM_PLATFORM_LINUX)
#include <dlfcn.h>
#endif

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
    //TODO: Init plugin
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
    if (info.element_entry_point) {
        //TODO: Use correct typedef for function pointer
        void (*cleanup)() = (void (*)())get_plugin_symbol(plugin, ELM_PLUGIN_EXIT_POINT);
        if (cleanup) {
            cleanup();
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