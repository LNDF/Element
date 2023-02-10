#pragma once

#include <utils/packed_set.h>

#include <utility>
#include <cereal/cereal.hpp>

namespace cereal {
    template <class Archive, typename K, typename H, typename E, typename A>
    inline void save(Archive& ar, const element::packed_set<K, H, E, A>& set) {
        ar(make_size_tag(static_cast<size_type>(set.size())));
        for (const auto& i : set) {
            ar(i);
        }
    }

    template <class Archive, typename K, typename H, typename E, typename A>
    inline void load(Archive& ar, element::packed_set<K, H, E, A>& set) {
        size_type size;
        ar(make_size_tag(size));
        set.clear();
        set.reserve(static_cast<std::size_t>(size));
        for (size_t i = 0; i < size; ++i) {
            typename element::packed_set<K, H, E, A>::key_type key;
            ar(key);
            set.emplace(std::move(key));
        }
    }
} // namespace cereal
