#pragma once

#include <functional>
#include <utils/packed_set.h>
#include <utils/pre_exec.h>

#define ELM_REGISTER_EVENT_CALLBACK(type, callback, priority) ELM_PRE_EXECUTE(element::event_manager::register_event_callback<type>, callback, priority)

namespace element {

    template<typename T>
    using event_callback_type = bool(*)(T&);

    enum event_callback_priority {
        lowest, low, medium, high, highest
    };

    namespace __detail {
        
        template<typename T>
        element::packed_set<event_callback_type<T>>& __event__manager_get_callbacks(event_callback_priority priority) {
            static element::packed_set<event_callback_type<T>> v_lowest;
            static element::packed_set<event_callback_type<T>> v_low;
            static element::packed_set<event_callback_type<T>> v_medium;
            static element::packed_set<event_callback_type<T>> v_high;
            static element::packed_set<event_callback_type<T>> v_highest;
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
        void register_event_callback(event_callback_type<T> callback, event_callback_priority priority) {
            __detail::__event__manager_get_callbacks<T>(priority).insert(callback);
        }

        template<typename T>
        void unregister_event_callback(event_callback_type<T> callback, event_callback_priority priority) {
            __detail::__event__manager_get_callbacks<T>(priority).erase(callback);
        }

        template<typename T>
        bool send_event(T& event) {
            for (event_callback_type<T>& callback : __detail::__event__manager_get_callbacks<T>(event_callback_priority::highest)) {
                if (!callback(event)) return false;
            }
            for (event_callback_type<T>& callback : __detail::__event__manager_get_callbacks<T>(event_callback_priority::high)) {
                if (!callback(event)) return false;
            }
            for (event_callback_type<T>& callback : __detail::__event__manager_get_callbacks<T>(event_callback_priority::medium)) {
                if (!callback(event)) return false;
            }
            for (event_callback_type<T>& callback : __detail::__event__manager_get_callbacks<T>(event_callback_priority::low)) {
                if (!callback(event)) return false;
            }
            for (event_callback_type<T>& callback : __detail::__event__manager_get_callbacks<T>(event_callback_priority::lowest)) {
                if (!callback(event)) return false;
            }
            return true;
        }

    } // namespace event_manager
    

} // namespace element
