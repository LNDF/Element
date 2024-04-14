#include "event_adapter.h"

using namespace element;

std::unordered_map<std::string, event_adapter_info>& element::__detail::__event_provider_get_event_providers() {
    static std::unordered_map<std::string, event_adapter_info> adapters;
    return adapters;
}