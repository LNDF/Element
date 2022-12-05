#pragma once

#include <tuple>
#include <unordered_map>
#include <utility>
#include <limits>
#include <functional>
#include <type_traits>

#include <ecs/component_pool.h>
#include <ecs/game_object.h>
#include <utils/uuid.h>
#include <utils/padded_array_view.h>
#include <utils/pointer_like_wrapper.h>

namespace element {

    template<bool is_const, typename... T>
    class scene_view_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = std::tuple<game_object*, T*...>;
            using difference_type = std::ptrdiff_t;
            using pointer = std::conditional_t<is_const, pointer_like_wrapper<const value_type>, pointer_like_wrapper<value_type>>;
            using reference = std::conditional_t<is_const, const value_type, value_type>;
        private:
            using tuples_type = std::conditional_t<is_const, const std::tuple<component_pool<T>*...>, std::tuple<component_pool<T>*...>>;
            using id_iter_type = std::conditional_t<is_const, padded_array_view<uuid>::const_iterator, padded_array_view<uuid>::iterator>;
            tuples_type* pools;
            id_iter_type it;

            template<std::size_t... I>
            bool is_valid_or_outside(const std::index_sequence<I...>&) const {
                if ((std::get<I>(*pools)->contains(*it) && ...) || !(std::get<I>(*pools)->contains(*it) || ...)) return true;
                return false;
            }

            template<std::size_t... I>
            reference get_tuple(const std::index_sequence<I...>) const {
                return std::make_tuple((*std::get<0>(*pools))[*it].first, &(*std::get<I>(*pools))[*it].second...);
            }
        public:
            scene_view_iterator(tuples_type* pools, id_iter_type it) : pools(pools), it(it) {
                while (!is_valid_or_outside(std::index_sequence_for<T...>{})) ++it;
            }

            reference operator*() const noexcept {
                return get_tuple(std::index_sequence_for<T...>{});
            }

            pointer operator->() const noexcept {
                return pointer(get_tuple(std::index_sequence_for<T...>{}));
            }

            scene_view_iterator& operator++() noexcept {
                do ++it; while (!is_valid_or_outside(std::index_sequence_for<T...>{}));
                return *this;
            }

            scene_view_iterator operator++(int) noexcept {
                scene_view_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            scene_view_iterator& operator--() noexcept {
                do --it; while (!is_valid_or_outside(std::index_sequence_for<T...>{}));
                return *this;
            }

            scene_view_iterator operator--(int) noexcept {
                scene_view_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            friend bool operator==(const scene_view_iterator& x, const scene_view_iterator& y) noexcept {
                return x.it == y.it;
            }

            friend bool operator!=(const scene_view_iterator& x, const scene_view_iterator& y) noexcept {
                return x.it != y.it;
            }
    };

    template<typename... T>
    class scene_view {
        public:
            using callback_type = std::function<void(game_object*, T*...)>;
            using iterator = scene_view_iterator<false, T...>;
            using const_iterator = scene_view_iterator<true, T...>;
        private:
            std::tuple<component_pool<T>*...> pools;
            padded_array_view<uuid> ids;

            template<std::size_t... I>
            void each_impl(const std::index_sequence<I...>&, const callback_type& callback) {
                for (const uuid& id : ids) {
                    bool call = true;
                    ((call = !std::get<I>(pools)->contains(id) ? false : call ? true : false), ...);
                    if (call) callback((*std::get<0>(pools))[id].first, &(*std::get<I>(pools))[id].second...);
                }
            }

            template<std::size_t... I>
            void sync_impl(const std::index_sequence<I...>&) {
                ids.size(std::numeric_limits<std::size_t>::max());
                ((ids = std::get<I>(pools)->size() >= ids.size() ? ids : std::get<I>(pools)->key_view()), ...);
            }
        public:
            scene_view(component_pool<T>*... pools)
                : pools{pools...} {
                    sync();
                }

            inline void each(const callback_type& callback) {
                each_impl(std::index_sequence_for<T...>{}, callback);
            }

            iterator begin() {
                return iterator(&pools, ids.begin());
            }

            const_iterator begin() const {
                return iterator(&pools, ids.begin());
            }

            const_iterator cbegin() const {
                return begin();
            }

            iterator end() {
                return iterator(&pools, ids.end());
            }

            const_iterator end() const {
                return iterator(&pools, ids.end());
            }

            const_iterator cend() const {
                return end();
            }

            inline void sync() {sync_impl(std::index_sequence_for<T...>{});}

            
    };
}