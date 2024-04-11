#pragma once

#include <utils/platform.h>
#include <filesystem>
#include <unordered_map>

namespace element {
    namespace plugins {
        
#ifdef ELM_PLATFORM_WINDOWS
        using handle = HMODULE;
#elif defined(ELM_PLATFORM_LINUX)
        using handle = void*;
#endif

    struct plugin_info {
        std::filesystem::path path;
        bool has_element_symbols;
    };

    const plugin_info* load_plugin(const std::filesystem::path& path);
    void unload_plugin(handle plugin);
    void unload_all_plugins();
    void* get_plugin_symbol(handle plugin, const char* symbol);
    const std::unordered_map<handle, plugin_info>& get_loaded_plugins();

    } // namespace plugins    
} // namespace element
