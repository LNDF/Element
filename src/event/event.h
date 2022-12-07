#pragma once

#include <functional>
#include <utility>
#include <typeindex>

#include <utils/packed_map.h>

namespace element {
    
    template<typename T>
    struct event_info {
        using callback_type = std::function<void(const T&)>;
        T event;
        bool cancelled = false;
        callback_type succes_callback;
        callback_type cancelled_callback;
        
    };

    class event_dispatcher_base {
        public:
            virtual ~event_dispatcher_base() = default;
            virtual void dispatch_all_events() = 0;
            virtual void finish_and_clear_all_events() = 0;
    };

    class event_manager;

    template<typename T>
    class event_dispatcher : public event_dispatcher_base {
            using callback_type = std::function<bool(const T&)>;
            using info_type = event_info<T>;

            friend class event_manager;

            callback_type default_listener;
            callback_type first_listener;
            std::vector<callback_type> listeners;
            std::vector<event_info<T>> events;

            inline bool maybe_call_and_cancel(callback_type cb, info_type& info) const {
                if (info.cancelled) return true;
                if (!cb(info.event)) {
                    //TODO: check cancel
                    info.cancelled = true;
                    return true;
                }
                return false;
            }

            virtual void dispatch_all_events() {
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

            virtual void finish_and_clear_all_events() {
                for (info_type& info : events) {
                    if (info.cancelled) {
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                    } else {
                        if (info.succes_callback) info.succes_callback(info.event);
                    }
                }
                events.clear();
            }

            virtual ~event_dispatcher() {
                
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

    class event_manager {
        private:
            static packed_map<std::type_index, event_dispatcher_base*> dispatchers;

            template<typename T>
            static event_dispatcher<T>* get_event_dispatcher() {
                std::type_index t(typeid(T));
                if (!dispatchers.contains(t)) {
                    event_dispatcher<T>* dispatcher = new event_dispatcher<T>();
                    dispatchers.try_emplace(t, dispatcher);
                    return dispatcher;
                }
                return reinterpret_cast<event_dispatcher<T>*>(dispatchers.at(t));
            }
        public:
            static void cleanup();

            template<typename T>
            static inline void register_listener(event_dispatcher<T>::callback_type listener) {
                get_event_dispatcher<T>()->register_listener(listener);
            }

            template<typename T>
            static inline void register_first_listener(event_dispatcher<T>::callback_type listener) {
                get_event_dispatcher<T>()->register_first_listener(listener);
            }

            template<typename T>
            static inline void register_default_listener(event_dispatcher<T>::callback_type listener) {
                get_event_dispatcher<T>()->register_default_listener(listener);
            }

            template<typename T>
            static inline void post_event(const T& event, event_dispatcher<T>::info_type::callback_type success = nullptr, event_dispatcher<T>::info_type::callback_type cancel = nullptr) {
                get_event_dispatcher<T>()->add_event_to_dispatch(event, success, cancel);
            }

            template<typename T>
            static inline void post_event(T&& event, event_dispatcher<T>::info_type::callback_type success = nullptr, event_dispatcher<T>::info_type::callback_type cancel = nullptr) {
                get_event_dispatcher<T>()->add_event_to_dispatch(std::move(event), success, cancel);
            }

            template<typename T>
            static inline bool send_event(const T& event) {
                return get_event_dispatcher<T>()->dispatch(event);
            }

            static void dispatch_queued_events();
    };
}