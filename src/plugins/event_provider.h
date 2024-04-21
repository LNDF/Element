#pragma once

#include <event/event.h>
#include <utils/pre_exec.h>
#include <functional>
#include <limits>
#include <unordered_map>
#include <string>

#define ELM_REGISTER_EVENT_ADAPTER(name, type, adapter, priority) ELM_PRE_EXECUTE(element::event_provider::register_adapter<type>, name, adapter, priority)

namespace element {
    
    using event_adapter_register_function = std::function<event_callback_handle(void*)>;
    using event_adapter_unregister_function = std::function<void(event_callback_handle)>;

    template<typename T>
    using event_adapter_function = std::function<bool(T&, void*)>;

    struct event_adapter_info {
        event_adapter_register_function register_function;
        event_adapter_unregister_function unregister_function;
    };

    namespace __detail {
        std::unordered_map<std::string, event_adapter_info>& __event_provider_get_event_providers();
    } // namespace __detail    

    namespace event_provider {
        
        template<typename T>
        void register_adapter(const std::string& name, event_adapter_function<T> adapter_function, event_callback_priority priority) {
            event_adapter_info info;
            info.register_function = [priority, adapter_function](void* function) {
                return event_manager::register_event_callback<T>([function, adapter_function](T& e){return adapter_function(e, function);}, priority);
            };
            info.unregister_function = [priority](event_callback_handle handle) {
                return event_manager::unregister_event_callback<T>(handle, priority);
            };
            __detail::__event_provider_get_event_providers()[name] = std::move(info);
        }

        template<typename T>
        void unregister_adapter(const std::string& name) {
            __detail::__event_provider_get_event_providers().erase(name);
        }

        event_callback_handle register_event(const std::string& name, void* function);
        void unregister_event(const std::string& name, event_callback_handle handle);

    } // namespace event_provider    

} // namespace element