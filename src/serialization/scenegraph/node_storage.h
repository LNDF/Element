#pragma once

#include <scenegraph/node_storage.h>
#include <utils/packed_map.h>
#include <utils/uuid.h>
#include <serialization/utils/packed_map.h>

namespace cereal {

    template<typename Archive, typename T>
    void save(Archive& ar, const element::scenegraph::node_storage<T>& storage) {
        ar(make_nvp("storage", storage.get_storage()));
    }

    template<typename Archive, typename T>
    void load(Archive& ar, element::scenegraph::node_storage<T>& storage) {
        element::packed_map<uuid, T> map;
        ar(make_nvp("storage", map));
        storage.set_storage(std::move(map));
    }

} // namespace cereal
