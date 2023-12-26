#pragma once

#include <utils/packed_map.h>

#include <utility>
#include <cereal/cereal.hpp>

namespace cereal {
    template <class Archive, typename K, typename V, typename H, typename E, typename A>
    void save(Archive& ar, const element::packed_map<K, V, H, E, A>& map) {
        ar(make_size_tag(static_cast<size_type>(map.size())));
        for (const auto& i : map) {
            ar(make_map_item(i.first, i.second));
        }
    }

    template <class Archive, typename K, typename V, typename H, typename E, typename A>
    void load(Archive& ar, element::packed_map<K, V, H, E, A>& map) {
        size_type size;
        ar(make_size_tag(size));
        map.clear();
        map.reserve(static_cast<std::size_t>(size));
        for (size_t i = 0; i < size; ++i) {
            typename element::packed_map<K, V, H, E, A>::key_type key;
            typename element::packed_map<K, V, H, E, A>::mapped_type value;
            ar(make_map_item(key, value));
            map.emplace(std::move(key), std::move(value));
        }
    }
} // namespace cereal
