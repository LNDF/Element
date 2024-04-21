#include "event_provider.h"

using namespace element;

std::unordered_map<std::string, event_adapter_info>& element::__detail::__event_provider_get_event_providers() {
    static std::unordered_map<std::string, event_adapter_info> adapters;
    return adapters;
}

event_callback_handle event_provider::register_event(const std::string& name, void* function) {
    auto& adapters = __detail::__event_provider_get_event_providers();
    auto it = adapters.find(name);
    if(it != adapters.end()) {
        return it->second.register_function(function);
    }
    return std::numeric_limits<event_callback_handle>::max();
}

void event_provider::unregister_event(const std::string& name, event_callback_handle handle) {
    auto& adapters = __detail::__event_provider_get_event_providers();
    auto it = adapters.find(name);
    if(it != adapters.end()) {
        it->second.unregister_function(handle);
    }
    
}