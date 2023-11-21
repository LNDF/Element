#pragma once

#include <render/material.h>
#include <serialization/utils/uuid.h>
#include <serialization/glm.h>
#include <serialization/vector.h>
#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace element {

    namespace __detail {
        template<class Archive, typename T>
        void __render_material_property_load(Archive& ar, render::material& mat, const render::material_property& prop) {
            try {
                std::uint32_t arr_size = prop.property->array_rows;
                std::uint32_t arr2d_size = prop.property->array_cols;
                if (arr_size > 0 || arr2d_size > 0) {
                    std::vector<T> vec;
                    ar(cereal::make_nvp(prop.property->name, vec));
                    mat.set_property_array(prop.property->name, &vec[0]);
                } else {
                    T t;
                    ar(cereal::make_nvp(prop.property->name, t));
                    mat.set_property(prop.property->name, t);
                }
            } catch (...) {}
        }

        template<class Archive, typename T>
        void __render_material_property_vec_mat_load(Archive& ar, render::material& mat, const render::material_property& prop) {
            switch(prop.property->columns) {
                case 0:
                    switch(prop.property->vecsize) {
                        case 0:
                            __render_material_property_load<Archive, T>(ar, mat, prop);
                            break;
                        case 1:
                            __render_material_property_load<Archive, glm::vec<1, T>>(ar, mat, prop);
                            break;
                        case 2:
                            __render_material_property_load<Archive, glm::vec<2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_load<Archive, glm::vec<3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_load<Archive, glm::vec<4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 2:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_load<Archive, glm::mat<2, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_load<Archive, glm::mat<2, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_load<Archive, glm::mat<2, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 3:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_load<Archive, glm::mat<3, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_load<Archive, glm::mat<3, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_load<Archive, glm::mat<3, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 4:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_load<Archive, glm::mat<4, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_load<Archive, glm::mat<4, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_load<Archive, glm::mat<4, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        template<class Archive>
        void __render_material_property_base_type_load(Archive& ar, render::material& mat, const render::material_property& prop) {
            switch(prop.property->type) {
                case render::shader_block_member_type::boolean_type:
                case render::shader_block_member_type::sint8_type:
                    __render_material_property_vec_mat_load<Archive, std::int8_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint16_type:
                    __render_material_property_vec_mat_load<Archive, std::int16_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint32_type:
                    __render_material_property_vec_mat_load<Archive, std::int32_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint64_type:
                    __render_material_property_vec_mat_load<Archive, std::int64_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint8_type:
                    __render_material_property_vec_mat_load<Archive, std::uint8_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint16_type:
                    __render_material_property_vec_mat_load<Archive, std::uint16_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint32_type:
                    __render_material_property_vec_mat_load<Archive, std::uint32_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint64_type:
                    __render_material_property_vec_mat_load<Archive, std::uint64_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::float32_type:
                    __render_material_property_vec_mat_load<Archive, float>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::float64_type:
                    __render_material_property_vec_mat_load<Archive, double>(ar, mat, prop);
                    break;
                default:
                    break;
            }
        }

        template<class Archive, typename T>
        void __render_material_property_save(Archive& ar, const render::material& mat, const render::material_property& prop) {
            try {
                std::uint32_t arr_size = prop.property->array_rows;
                std::uint32_t arr2d_size = prop.property->array_cols;
                if (arr2d_size > 0) arr_size *= arr2d_size;
                if (arr_size > 0) {
                    std::vector<T> vec;
                    vec.resize(arr_size);
                    mat.set_property_array(prop.property->name, &vec[0]);
                    ar(cereal::make_nvp(prop.property->name, vec));
                } else {
                    T t;
                    mat.get_property(prop.property->name, t);
                    ar(cereal::make_nvp(prop.property->name, t));
                }
            } catch (...) {}
        }

        template<class Archive, typename T>
        void __render_material_property_vec_mat_save(Archive& ar, const render::material& mat, const render::material_property& prop) {
            switch(prop.property->columns) {
                case 0:
                    switch(prop.property->vecsize) {
                        case 0:
                            __render_material_property_save<Archive, T>(ar, mat, prop);
                            break;
                        case 1:
                            __render_material_property_save<Archive, glm::vec<1, T>>(ar, mat, prop);
                            break;
                        case 2:
                            __render_material_property_save<Archive, glm::vec<2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_save<Archive, glm::vec<3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_save<Archive, glm::vec<4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 2:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_save<Archive, glm::mat<2, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_save<Archive, glm::mat<2, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_save<Archive, glm::mat<2, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 3:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_save<Archive, glm::mat<3, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_save<Archive, glm::mat<3, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_save<Archive, glm::mat<3, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                case 4:
                    switch(prop.property->vecsize) {
                        case 2:
                            __render_material_property_save<Archive, glm::mat<4, 2, T>>(ar, mat, prop);
                            break;
                        case 3:
                            __render_material_property_save<Archive, glm::mat<4, 3, T>>(ar, mat, prop);
                            break;
                        case 4:
                            __render_material_property_save<Archive, glm::mat<4, 4, T>>(ar, mat, prop);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        template<class Archive>
        void __render_material_property_base_type_save(Archive& ar, const render::material& mat, const render::material_property& prop) {
            switch(prop.property->type) {
                case render::shader_block_member_type::boolean_type:
                case render::shader_block_member_type::sint8_type:
                    __render_material_property_vec_mat_save<Archive, std::int8_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint16_type:
                    __render_material_property_vec_mat_save<Archive, std::int16_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint32_type:
                    __render_material_property_vec_mat_save<Archive, std::int32_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::sint64_type:
                    __render_material_property_vec_mat_save<Archive, std::int64_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint8_type:
                    __render_material_property_vec_mat_save<Archive, std::uint8_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint16_type:
                    __render_material_property_vec_mat_save<Archive, std::uint16_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint32_type:
                    __render_material_property_vec_mat_save<Archive, std::uint32_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::uint64_type:
                    __render_material_property_vec_mat_save<Archive, std::uint64_t>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::float32_type:
                    __render_material_property_vec_mat_save<Archive, float>(ar, mat, prop);
                    break;
                case render::shader_block_member_type::float64_type:
                    __render_material_property_vec_mat_save<Archive, double>(ar, mat, prop);
                    break;
                default:
                    break;
            }
        }
    }
}

namespace cereal { 
    template <class Archive>
    void serialize(Archive& ar, element::render::material_buffer& buffer) {
        ar(make_nvp("data", buffer.data));
        ar(make_nvp("needs_sync", buffer.needs_sync));
        ar(make_nvp("set", buffer.set));
        ar(make_nvp("binding", buffer.binding));
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void load(Archive& ar, std::pair<element::render::material*, const std::unordered_map<std::string, element::render::material_property>*>& data) {
        for (const auto& [name, property] : *data.second) {
            element::__detail::__render_material_property_base_type_load(ar, *data.first, property);
        }
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void save(Archive& ar, const std::pair<element::render::material*, const std::unordered_map<std::string, element::render::material_property>*>& data) {
        for (const auto& [name, property] : *data.second) {
            element::__detail::__render_material_property_base_type_save(ar, *data.first, property);
        }
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void load(Archive& ar, element::render::material& material) {
        element::uuid pipeline_id;
        ar(make_nvp("pipeline_id", pipeline_id));
        material.set_pipeline_id(pipeline_id);
        material.init(true);
        auto properties = std::make_pair(&material, &material.get_properties());
        ar(make_nvp("properties", properties));
    }

    template <class Archive,
        cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value>
        = cereal::traits::sfinae>
    void save(Archive& ar, const element::render::material& material) {
        ar(make_nvp("pipeline_id", material.get_pipeline_id()));
        auto properties = std::make_pair(&material, &material.get_properties());
        ar(make_nvp("properties", properties));
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