#pragma once

#include <tuple>
#include <unordered_map>
#include <utility>
#include <limits>

#include "componentpool.h"
#include "gameobject.h"
#include "../utils/uuid.h"

namespace engine {
    template<typename... T>
    class scene_view {
        private:
            std::tuple<component_pool<T>*...> pools;
            component_pool_base* small;

            template<std::size_t C, std::size_t... I, typename CB>
            void each_impl(std::index_sequence<I...>, const CB& callback) {
                auto pool = std::get<C>(pools);
                for (auto& i : *pool) {
                    bool call = true;
                    uuid id = i.first->get_uuid();
                    ((call = !std::get<I>(pools)->contains(id) ? false : call ? true : false), ...);
                    if (call) callback(i.first, &(*std::get<I>(pools))[id].second...);
                }
            }

            template<std::size_t... I, typename C>
            void each_small(std::index_sequence<I...> s, const C& callback) {
                (((component_pool_base*)std::get<I>(pools) == small ? each_impl<I>(s, callback) : void()), ...);
            }

            template<std::size_t... I>
            void update_impl(std::index_sequence<I...>) {
                std::size_t s = std::numeric_limits<std::size_t>::max();
                ((small = std::get<I>(pools)->size() >= s ? small : std::get<I>(pools)), ...);
            }
        public:
            scene_view(component_pool<T>*... pools)
                : pools{pools...} {
                    update();
                }

            template<typename C>
            void each(const C& callback) {
                each_small(std::index_sequence_for<T...>{}, callback);
            }

            inline void update() {update_impl(std::index_sequence_for<T...>{});}
    };
}