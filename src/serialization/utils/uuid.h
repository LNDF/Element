#pragma once

#include <utils/uuid.h>

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>

namespace cereal {
    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline std::string save_minimal(const Archive&, const element::uuid& id) {
        return id.str(); 
    }
    
    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline  void load_minimal(const Archive&, element::uuid& id, const std::string& s) {
        id.set_from_str(s);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void save(Archive& ar, const element::uuid& id) {
        ar.saveBinary(id.bytes, 16);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void load(Archive& ar, element::uuid& id) {
        ar.loadBinary(id.bytes, 16);
    }
} // namespace cereal
