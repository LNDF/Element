#pragma once

#include <utils/uuid.h>
#include <scenegraph/node_ref.h>
#include <serialization/utils/uuid.h>

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>

namespace cereal {
    template <class Archive>
    void save(Archive& ar, const element::scenegraph::node_ref& ref) {
        ar(make_nvp("id", ref.get_id()));
    }

    template <class Archive>
    void load(Archive& ar, element::scenegraph::node_ref& ref) {
        element::uuid id;
        ar(make_nvp("id", id));
        ref.set_id(id);
    }

    template <class Archive, typename T>
    void save(Archive& ar, const element::scenegraph::node_ref_derived<T>& ref) {
        ar(make_nvp("id", ref.get_id()));
    }

    template <class Archive, typename T>
    void load(Archive& ar, element::scenegraph::node_ref_derived<T>& ref) {
        element::uuid id;
        ar(make_nvp("id", id));
        ref.set_id(id);
    }
} // namespace cereal
