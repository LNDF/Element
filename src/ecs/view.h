#pragma once

#include <tuple>
#include <unordered_map>
#include <utility>
#include <limits>
#include <functional>

#include "component_pool.h"
#include "game_object.h"
#include "../utils/uuid.h"

namespace engine {

    template<typename... T>
    class scene_view_iterator {
        private:
            std::tuple<component_pool<T>*...>* pools;
            component_pool_base* small;
            std::size_t index;
            
            template<std::size_t... I, std::size_t C>
            bool valid_index_impl(const std::index_sequence<I...>&, std::size_t index) const {
                auto pool = std::get<C>(*pools);
                
            }

            template<std::size_t... I>
            bool valid_index(const std::index_sequence<I...>& s, std::size_t index) const {
                bool valid = false;
                ((valid = (component_pool_base*)std::get<I>(*pools) == small ? valid_index_impl<I>(s, index) : valid), ...);
                return valid;
            }
        public:
            scene_view_iterator(std::tuple<component_pool<T>*...>* pools, component_pool_base* small, std::size_t start)
                : pools(pools), small(small)
                   {}

            
    };

    template<typename... T>
    class scene_view {
        public:
            using callback_type = std::function<void(game_object*, T*...)>;
            using iterator_type = scene_view_iterator<T...>;
        private:
            std::tuple<component_pool<T>*...> pools;
            component_pool_base* small;

            template<std::size_t C, std::size_t... I>
            void each_impl(const std::index_sequence<I...>&, const callback_type& callback) {
                auto pool = std::get<C>(pools);
                for (auto& [id, comp] : *pool) {
                    bool call = true;
                    ((call = !std::get<I>(pools)->contains(id) ? false : call ? true : false), ...);
                    if (call) callback(comp.first, &(*std::get<I>(pools))[id].second...);
                }
            }

            template<std::size_t... I>
            void each_small(const std::index_sequence<I...>& s, const callback_type& callback) {
                (((component_pool_base*)std::get<I>(pools) == small ? each_impl<I>(s, callback) : void()), ...);
            }

            template<std::size_t... I>
            void sync_impl(const std::index_sequence<I...>&) {
                std::size_t s = std::numeric_limits<std::size_t>::max();
                ((small = std::get<I>(pools)->size() >= s ? small : std::get<I>(pools)), ...);
            }
        public:
            scene_view(component_pool<T>*... pools)
                : pools{pools...} {
                    sync();
                }

            inline void each(const callback_type& callback) {
                each_small(std::index_sequence_for<T...>{}, callback);
            }

            inline void sync() {sync_impl(std::index_sequence_for<T...>{});}
    };
}