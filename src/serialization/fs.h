#pragma once

#include <core/fs.h>

#include <cereal/cereal.hpp>

namespace cereal {
    
    template<class Archiver>
    void serialize(Archiver& ar, element::fs_resource_info& info) {
        ar(cereal::make_nvp("path", info.path), cereal::make_nvp("type", info.type));
    }

} // namespace cereal
