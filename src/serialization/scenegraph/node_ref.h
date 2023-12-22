#pragma once

#include <utils/uuid.h>
#include <scenegraph/node_ref.h>
#include <serialization/defs.h>
#include <serialization/utils/uuid.h>

#include <cereal/access.hpp>
#include <cereal/details/traits.hpp>

namespace cereal {
    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline std::string save_minimal(const Archive&, const element::scenegraph::node_ref& ref) {
        return ref.get_id().str(); 
    }
    
    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline  void load_minimal(const Archive&, element::scenegraph::node_ref& ref, const std::string& s) {
        element::uuid id(s);
        ref.set_id(id);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void save(Archive& ar, const element::scenegraph::node_ref& ref) {
        ar.saveBinary(ref.get_id().bytes, 16);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void load(Archive& ar, element::scenegraph::node_ref& ref) {
        element::uuid id;
        ar.loadBinary(id.bytes, 16);
        ref.set_id(id);
    }

    template <class Archive, typename T,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline std::string save_minimal(const Archive&, const element::scenegraph::node_ref_derived<T>& ref) {
        return ref.get_id().str(); 
    }
    
    template <class Archive, typename T,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline  void load_minimal(const Archive&, element::scenegraph::node_ref_derived<T>& ref, const std::string& s) {
        element::uuid id(s);
        ref.set_id(id);
    }

    template <class Archive, typename T,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void save(Archive& ar, const element::scenegraph::node_ref_derived<T>& ref) {
        ar.saveBinary(ref.get_id().bytes, 16);
    }

    template <class Archive, typename T,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void load(Archive& ar, element::scenegraph::node_ref_derived<T>& ref) {
        element::uuid id;
        ar.loadBinary(id.bytes, 16);
        ref.set_id(id);
    }
} // namespace cereal
