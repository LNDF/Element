#pragma once

#include <render/shader_layout.h>
#include <serialization/vector.h>

#include <cereal/cereal.hpp>

namespace cereal {
    
    template<class Archiver>
    void serialize(Archiver& ar, element::render::shader_block_member& member) {
        ar(make_nvp("name", member.name));
        ar(make_nvp("type", member.type));
        ar(make_nvp("size", member.size));
        ar(make_nvp("offset", member.offset));
        ar(make_nvp("array_stride", member.array_stride));
        ar(make_nvp("matrix_stride", member.matrix_stride));
        ar(make_nvp("vecsize", member.vecsize));
        ar(make_nvp("columns", member.columns));
        ar(make_nvp("array_rows", member.array_rows));
        ar(make_nvp("array_cols", member.array_cols));
    }

    template<class Archiver>
    void serialize(Archiver& ar, element::render::shader_resource_layout& res) {
        ar(make_nvp("member_data", cereal::base_class<element::render::shader_block_member>(&res)));
        ar(make_nvp("set", res.set));
        ar(make_nvp("binding", res.binding));
        ar(make_nvp("sampler_resource",res.sampler_resource));
        ar(make_nvp("members", res.members));
    }

    template<class Archiver>
    void serialize(Archiver& ar, element::render::shader_layout& layout) {
        ar(make_nvp("push_constants", layout.push_constants));
        ar(make_nvp("descriptor_sets", layout.descriptor_sets));
    }

} // namespace cereal
