#pragma once

#include <utils/uuid.h>

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>

namespace cereal {
    // template <class Archive
    //     // , cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
    //     // = cereal::traits::sfinae
    // >
    // std::string save_minimal(const Archive&, const element::uuid& id) {
    //     return id.str(); 
    // }
    
    // template <class Archive
    //     // , cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
    //     // = cereal::traits::sfinae
    // >
    // void load_minimal(const Archive&, element::uuid& id, const std::string& s) {
    //     id.set_from_str(s);
    // }

    template <class Archive
        // , cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        // = cereal::traits::sfinae
    >
    void save(Archive& ar, const element::uuid& id) {
        ar(make_nvp("low", id.low));
        ar(make_nvp("high", id.high));
    }

    template <class Archive
        // , cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        // = cereal::traits::sfinae
    >
    void load(Archive& ar, element::uuid& id) {
        ar(make_nvp("low", id.low));
        ar(make_nvp("high", id.high));
    }
} // namespace cereal
