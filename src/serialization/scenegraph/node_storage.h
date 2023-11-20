#pragma once

#include <scenegraph/node_storage.h>
#include <utils/packed_map.h>
#include <utils/uuid.h>
#include <serialization/utils/packed_map.h>

namespace cereal {

    template<typename Archive, typename T, typename H, typename E, typename A>
    void save(Archive& ar, const element::packed_map<element::uuid, T, H, E, A>& storage) {
        ar(make_size_tag(static_cast<size_type>(storage.size())));
        for (const auto& [id, node] : storage) {
            ar(node);
        }
    }

    template<typename Archive, typename T, typename H, typename E, typename A>
    void load(Archive& ar, element::packed_map<element::uuid, T, H, E, A>& storage) {
        size_type size;
        ar(make_size_tag(size));
        storage.clear();
        storage.reserve(static_cast<std::size_t>(size));
        for (size_type i = 0; i < size; ++i) {
            T value;
            ar(value);
            storage.emplace(std::move(value.get_id()), std::move(value));
        }
    }

    template<typename Archive, typename T>
    void save(Archive& ar, const element::scenegraph::node_storage<T>& storage) {
        ar(make_nvp("storage", storage.get_storage()));
    }

    template<typename Archive, typename T>
    void load(Archive& ar, element::scenegraph::node_storage<T>& storage) {
        element::packed_map<element::uuid, T> map;
        ar(make_nvp("storage", map));
        storage.__set_storage(std::move(map));
    }

} // namespace cereal
