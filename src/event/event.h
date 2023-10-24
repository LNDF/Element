#pragma once

#include <utility>
#include <functional>
#include <utils/packed_map.h>
#include <utils/pre_exec.h>

#define ELM_REGISTER_EVENT_CALLBACK(type, callback, priority) ELM_PRE_EXECUTE(element::event_manager::register_event_callback<type>, callback, priority)

namespace element {

    using event_callback_handle = std::uint32_t;

    template<typename T>
    using event_callback = std::function<bool(T&)>;

    enum event_callback_priority {
        lowest, low, medium, high, highest
    };

    namespace __detail {
        
        extern event_callback_handle __event_manager_handle_counter;

        template<typename T>
        element::packed_map<event_callback_handle, event_callback<T>>& __event__manager_get_callbacks(event_callback_priority priority) {
            static element::packed_map<event_callback_handle, event_callback<T>> v_lowest;
            static element::packed_map<event_callback_handle, event_callback<T>> v_low;
            static element::packed_map<event_callback_handle, event_callback<T>> v_medium;
            static element::packed_map<event_callback_handle, event_callback<T>> v_high;
            static element::packed_map<event_callback_handle, event_callback<T>> v_highest;
            switch (priority) {
                case lowest:
                    return v_lowest;
                case low:
                    return v_low;
                case medium:
                    return v_medium;
                case high:
                    return v_high;
                case highest:
                    return v_highest;
                default:
                    return v_medium;
            }
        }

    } // namespace __detail

    namespace event_manager {
        
        template<typename T>
        event_callback_handle register_event_callback(event_callback<T> callback, event_callback_priority priority) {
            event_callback_handle handle = __detail::__event_manager_handle_counter++;
            __detail::__event__manager_get_callbacks<T>(priority).insert(std::make_pair(handle, callback));
            return handle;
        }

        template<typename T>
        void unregister_event_callback(event_callback_handle handle, event_callback_priority priority) {
            __detail::__event__manager_get_callbacks<T>(priority).erase(handle);
        }

        template<typename T>
        bool send_event(T& event) {
            for (auto& [handle, callback] : __detail::__event__manager_get_callbacks<T>(event_callback_priority::highest)) {
                if (!callback(event)) return false;
            }
            for (auto& [handle, callback] : __detail::__event__manager_get_callbacks<T>(event_callback_priority::high)) {
                if (!callback(event)) return false;
            }
            for (auto& [handle, callback] : __detail::__event__manager_get_callbacks<T>(event_callback_priority::medium)) {
                if (!callback(event)) return false;
            }
            for (auto& [handle, callback] : __detail::__event__manager_get_callbacks<T>(event_callback_priority::low)) {
                if (!callback(event)) return false;
            }
            for (auto& [handle, callback]
             : __detail::__event__manager_get_callbacks<T>(event_callback_priority::lowest)) {
                if (!callback(event)) return false;
            }
            return true;
        }

    } // namespace event_manager
    

} // namespace element
