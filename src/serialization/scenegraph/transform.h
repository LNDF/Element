#pragma once

#include <scenegraph/transform.h>
#include <serialization/glm.h>
#include <cereal/cereal.hpp>

namespace cereal {

    template<typename Archive>
    void save(Archive& ar, const element::scenegraph::transform& transform) {
        ar(make_nvp("position", transform.get_position()));
        ar(make_nvp("rotation", transform.get_rotation()));
        ar(make_nvp("scale", transform.get_scale()));
    }

    template<typename Archive>
    void load(Archive& ar, element::scenegraph::transform& transform) {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        ar(make_nvp("position", position));
        ar(make_nvp("rotation", rotation));
        ar(make_nvp("scale", scale));
        transform.set_position(position);
        transform.set_rotation(rotation);
        transform.set_scale(scale);
    }
} // namespace cereal
