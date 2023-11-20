#pragma once

#include <render/material.h>
#include <serialization/utils/uuid.h>
#include <serialization/vector.h>
#include <cereal/cereal.hpp>

namespace cereal { 
    template <class Archive>
    void serialize(Archive& ar, element::render::material_buffer& buffer) {
        ar(make_nvp("data", buffer.data));
        ar(make_nvp("needs_sync", buffer.needs_sync));
        ar(make_nvp("set", buffer.set));
        ar(make_nvp("binding", buffer.binding));
    }

    template <class Archive>
    void serialize(Archive& ar, element::render::material_property& property) {
        ar(make_nvp("buffer_index", property.buffer_index));
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void load(Archive& ar, element::render::material& material) {
        //TODO
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void save(Archive& ar, const element::render::material& material) {
        //TODO
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void load(Archive& ar, element::render::material& material) {
        element::uuid pipeline_id;
        std::vector<element::render::material_buffer> uniform_buffers;
        element::render::material_buffer push_constants_buffer;
        ar(pipeline_id);
        ar(uniform_buffers);
        ar(push_constants_buffer);
        push_constants_buffer.needs_sync = true;
        for (auto& buffer : uniform_buffers) {
            buffer.needs_sync = true;
        }
        material.set_pipeline_id(pipeline_id);
        material.__set_uniform_buffer(std::move(uniform_buffers));
        material.__set_push_constants_buffer(std::move(push_constants_buffer));
        material.init(false);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void save(Archive& ar, const element::render::material& material) {
        ar(material.get_pipeline_id());
        ar(material.get_uniform_buffers());
        ar(material.get_push_constants_buffer());
    }
}