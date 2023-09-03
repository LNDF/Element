#pragma once

#include <functional>
#include <utility>
#include <typeindex>

#include <utils/packed_map.h>

namespace element {
    
    namespace __detail {

        template<typename T>
        struct __event_manager_info {
            using callback_type = std::function<void(const T&)>;
            T event;
            bool cancelled = false;
            callback_type succes_callback;
            callback_type cancelled_callback;
        };

        class __event_manager_dispatcher_base {
            public:
                virtual ~__event_manager_dispatcher_base() = default;
                virtual void dispatch_all_events() = 0;
                virtual void finish_and_clear_all_events() = 0;
        };

        template<typename T>
        class __event_manager_dispatcher : public __event_manager_dispatcher_base {
            public:
                using callback_type = std::function<bool(const T&)>;
                using info_type = __event_manager_info<T>;
            private:
                std::vector<callback_type> listeners;
                std::vector<info_type> events;
            public:
                callback_type default_listener;
                callback_type first_listener;

                inline bool maybe_call_and_cancel(callback_type cb, info_type& info) const {
                    if (info.cancelled) return true;
                    if (!cb(info.event)) {
                        //TODO: check cancel
                        info.cancelled = true;
                        return true;
                    }
                    return false;
                }

                void dispatch_all_events() final override {
                    if (first_listener) {
                        for (info_type& info : events) {
                            maybe_call_and_cancel(first_listener, info);
                        }
                    }
                    for (info_type& info : events) {
                        for (callback_type listener : listeners) {
                            if (maybe_call_and_cancel(listener, info)) break;
                        }
                    }
                    if (default_listener) {
                        for (info_type& info : events) {
                            maybe_call_and_cancel(default_listener, info);
                        }
                    }
                }

                void finish_and_clear_all_events() final override {
                    for (info_type& info : events) {
                        if (info.cancelled) {
                            if (info.cancelled_callback) info.cancelled_callback(info.event);
                        } else {
                            if (info.succes_callback) info.succes_callback(info.event);
                        }
                    }
                    events.clear();
                }

                ~__event_manager_dispatcher() final override {
                    
                }

                bool dispatch(const T& event) const {
                    if (first_listener) {
                        if (!first_listener(event)) {
                            return false;
                        }
                    }
                    for (callback_type listener : listeners) {
                        if (!listener(event)) {
                            return false;
                        }
                    }
                    if (default_listener) {
                        if (!default_listener(event)) {
                            return false;
                        }
                    }
                    return true;
                }

                void register_listener(callback_type listener) {
                    listeners.push_back(listener);
                }

                void register_first_listener(callback_type listener) {
                    if (!first_listener) first_listener = listener;
                }

                void register_default_listener(callback_type listener) {
                    if (!default_listener) default_listener = listener;
                }

                void add_event_to_dispatch(const T& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                    events.emplace_back(event, false, success, cancel);
                }

                void add_event_to_dispatch(T&& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                    events.emplace_back(std::move(event), false, success, cancel);
                }
        };

        extern packed_map<std::type_index, __detail::__event_manager_dispatcher_base*> __event_manager_dispatchers;

        template<typename T>
        __detail::__event_manager_dispatcher<T>* __event_manager_get_dispatcher() {
            std::type_index t(typeid(T));
            try {
                return reinterpret_cast<__detail::__event_manager_dispatcher<T>*>(__detail::__event_manager_dispatchers.at(t));
            } catch (const std::out_of_range&) {
                __detail::__event_manager_dispatcher<T>* dispatcher = new __detail::__event_manager_dispatcher<T>();
                __detail::__event_manager_dispatchers.try_emplace(t, dispatcher);
                return dispatcher;
            }
        }

    } // namespace __detail
    

    namespace event_manager {
        template<typename T>
        using event_callback_type = typename __detail::__event_manager_dispatcher<T>::info_type::callback_type;

        template<typename T>
        using listener_callback_type = typename __detail::__event_manager_dispatcher<T>::callback_type;

        void cleanup();


        template<typename T>
        static inline void register_listener(listener_callback_type<T> listener) {
            __detail::__event_manager_get_dispatcher<T>()->register_listener(listener);
        }

        template<typename T>
        static inline void register_first_listener(listener_callback_type<T> listener) {
           __detail::__event_manager_get_dispatcher<T>()->register_first_listener(listener);
        }

        template<typename T>
        static inline void register_default_listener(listener_callback_type<T>listener) {
            __detail::__event_manager_get_dispatcher<T>()->register_default_listener(listener);
        }

        template<typename T>
        static inline void post_event(const T& event, event_callback_type<T> success = nullptr, event_callback_type<T> cancel = nullptr) {
            __detail::__event_manager_get_dispatcher<T>()->add_event_to_dispatch(event, success, cancel);
        }

        template<typename T>
        static inline void post_event(T&& event, event_callback_type<T> success = nullptr, event_callback_type<T> cancel = nullptr) {
            __detail::__event_manager_get_dispatcher<T>()->add_event_to_dispatch(std::move(event), success, cancel);
        }

        template<typename T>
        static inline bool send_event(const T& event) {
            return __detail::__event_manager_get_dispatcher<T>()->dispatch(event);
        }

        void dispatch_queued_events();
    } // namespace event

}