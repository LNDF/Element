#pragma once

#include <utils/packed_map.h>
#include <functional>
#include <utility>

namespace engine {
    
    template<typename T>
    struct event_info {
        using callback_type = std::function<bool(T&)>;
        T event;
        bool cancelled = false;
        callback_type succes_callback;
        callback_type cancelled_callback;
        
    };

    class event_dispatcher_base {
        public:
            virtual void dispatch_all_events() = 0;
            virtual void finish_and_clear_all_events() = 0;
    };

    template<typename T>
    class event_dispatcher : public event_dispatcher_base {
        public:
            using callback_type = std::function<bool(T&)>;
            using info_type = event_info<T>;
        private:
            callback_type default_listener;
            callback_type first_listener;
            std::vector<callback_type> listeners;
            std::vector<event_info<T>> events;

            virtual void dispatch_all_events() {
                if (first_listener) {
                    for (info_type& info : events) {
                        if (!first_listener(info.event)) {
                            //TODO: check cancel
                            info.cancelled = true;
                        }
                    }
                }
                for (info_type& info : events) {
                    if (info.cancelled) continue;;
                    for (callback_type listener : listeners) {
                        if (!listener(info.event)) {
                            //TODO: cancel check
                            info.cancelled = true;
                            break;
                        }
                    }
                }
                if (default_listener) {
                    for (info_type& info : events) {
                        if (!info.cancelled) {
                            if (!default_listener(info.event)) {
                                //TODO: check cancel
                                info.cancelled = true;
                            }
                        }
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

            void dispatch(info_type& info) {
                if (first_listener) {
                    if (!first_listener(info.event)) {
                        //TODO: check cancel
                        info.cancelled = true;
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                for (callback_type listener : listeners) {
                    if (!listener(info.event)) {
                        //TODO: check cancel
                        info.cancelled = true;
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                if (default_listener) {
                    if (!default_listener(info.event)) {
                        //TODO: check cancel
                        info.cancelled = true;
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                if (info.succes_callback) info.succes_callback(info.event);
            }

            void dispatch(info_type&& info) {
                if (first_listener) {
                    if (!first_listener(info.event)) {
                        //TODO: check cancel
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                for (callback_type listener : listeners) {
                    if (!listener(info.event)) {
                        //TODO: check cancel
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                if (default_listener) {
                    if (!default_listener(info.event)) {
                        //TODO: check cancel
                        if (info.cancelled_callback) info.cancelled_callback(info.event);
                        return;
                    }
                }
                if (info.succes_callback) info.succes_callback(info.event);
            }

            void dispatch(const T& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                return dispatch({event, false, nullptr, nullptr});
            }

            void dispatch(T&& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                return dispatch({std::move(event), false, nullptr, nullptr});
            }

            void register_listener(callback_type listener) {
                listeners.push_back(listener);
            }

            void register_first_listener(callback_type listener) {
                first_listener = listener;
            }

            void register_default_listener(callback_type listener) {
                default_listener = listener;
            }

            void add_event_to_dispatch(const T& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                events.emplace_back(event, false, nullptr, nullptr);
            }

            void add_event_to_dispatch(T&& event, info_type::callback_type success = nullptr, info_type::callback_type cancel = nullptr) {
                events.emplace_back(std::move(event), false, nullptr, nullptr);
            }
    };

    class event_manager {
        private:

        public:

    };
}

