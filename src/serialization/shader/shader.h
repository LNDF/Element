#pragma once

#include <shader/shader.h>
#include <serialization/shader/layout.h>
#include <serialization/vector.h>
#include <cereal/cereal.hpp>

namespace cereal {
    
    template<typename Archive>
    void serialize(Archive& ar, element::shader::shader_data& shader) {
        ar(make_nvp("spv", shader.spv));
        ar(make_nvp("layout", shader.layout));
    }

} // namespace cereal
