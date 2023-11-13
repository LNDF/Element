#pragma once

#include <render/pipeline.h>
#include <serialization/render/shader_layout.h>
#include <serialization/utils/uuid.h>
#include <serialization/vector.h>
#include <serialization/pair.h>

#include <cereal/cereal.hpp>

namespace element {
    namespace __detail {
        
        template<class Archive>
        void __render_pipeline_data_common_serialize(Archive& ar, render::pipeline_data& data) {
            ar(cereal::make_nvp("vert_id", data.vert_id));
            ar(cereal::make_nvp("frag_id", data.frag_id));
            ar(cereal::make_nvp("backface_culling", data.backface_culling));
            ar(cereal::make_nvp("frontface", data.frontface));
            ar(cereal::make_nvp("transparent", data.transparent));
        }

    } // namespace __detail
    
} // namespace element


namespace cereal {

    template<class Archive>
    std::uint32_t save_minimal(Archive& ar, const vk::ShaderStageFlags& stages) {
        return stages.m_mask;
    }

    template<class Archive>
    void load_minimal(Archive& ar, vk::ShaderStageFlags& stages, const std::uint32_t& mask) {
        stages.m_mask = mask;
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void serialize(Archive& ar, element::render::pipeline_data& data) {
        element::__detail::__render_pipeline_data_common_serialize(ar, data);
    }

    template <class Archive,
        cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void serialize(Archive& ar, element::render::pipeline_data& data) {
        element::__detail::__render_pipeline_data_common_serialize(ar, data);
        ar(make_nvp("layouts", data.layouts));
        ar(make_nvp("push_constants", data.push_constants));
    }
} // namespace cereal
