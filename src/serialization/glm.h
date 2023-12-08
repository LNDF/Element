#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace element {
    namespace __detail {
        static const char* __serialization_glm_member_names[4] = {"x", "y", "z", "w"};
    } // namespace __detail
} // namespace element


namespace cereal {

    template<class Archive, glm::length_t L, typename T, glm::qualifier Q>
    void serialize(Archive& ar, glm::vec<L, T, Q>& vec) {
        for (glm::length_t i = 0; i < L; ++i) {
            ar(make_nvp(element::__detail::__serialization_glm_member_names[i], vec[i]));
        }
    }

    template<class Archive, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void serialize(Archive& ar, glm::mat<C, R, T, Q>& mat) {
        for (glm::length_t i = 0; i < C; ++i) {
            ar(make_nvp(element::__detail::__serialization_glm_member_names[i], mat[i]));
        }
    }
}