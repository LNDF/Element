#pragma once

#include <render/mesh.h>

#include <cereal/cereal.hpp>

namespace cereal {
    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void save(Archive& ar, const element::render::mesh& mesh) {
        std::size_t vert_count = mesh.vertices.size();
        std::size_t ind_count = mesh.vertices.size();
        ar(vert_count);
        ar(ind_count);
        ar.saveBinary(&mesh.vertices[0], vert_count * sizeof(element::render::vertex));
        ar.saveBinary(&mesh.indices[0], ind_count * sizeof(std::uint32_t));
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    inline void load(Archive& ar, element::render::mesh& mesh) {
        std::size_t vert_count;
        std::size_t ind_count;
        mesh.vertices.clear();
        mesh.indices.clear();
        ar(vert_count);
        ar(ind_count);
        mesh.vertices.resize(vert_count);
        mesh.indices.resize(ind_count);
        ar.loadBinary(&mesh.vertices[0], vert_count * sizeof(element::render::vertex));
        ar.loadBinary(&mesh.indices[0], ind_count * sizeof(std::uint32_t));
    }
} //namespace cereal