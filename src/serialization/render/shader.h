#pragma once

#include <render/shader.h>
#include <serialization/render/shader_layout.h>
#include <serialization/vector.h>
#include <cereal/cereal.hpp>

namespace cereal {
    
    template<typename Archive>
    void serialize(Archive& ar, element::render::shader& shader) {
        ar(make_nvp("spv", shader.spv));
        ar(make_nvp("reflect", shader.reflect));
    }

} // namespace cereal
