#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace cereal {

    template<class Archive, glm::length_t L, typename T, glm::qualifier Q>
    void save(Archive& ar, const glm::vec<L, T, Q>& vec) {
        ar(make_size_tag(static_cast<size_type>(L)));
        for (glm::length_t i = 0; i < L; ++i) {
            ar(vec[i]);
        }
    }

    template<class Archive, glm::length_t L, typename T, glm::qualifier Q>
    void load(Archive& ar, glm::vec<L, T, Q>& vec) {
        size_type size;
        ar(make_size_tag(size));
        glm::length_t vecsize;
        if (L < size) {
            vecsize = L;
        } else {
            vecsize = static_cast<glm::length_t>(size);
        }
        for (glm::length_t i = 0; i < vecsize; ++i) {
            ar(vec[i]);
        }
    }

    template<class Archive, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void save(Archive& ar, const glm::mat<C, R, T, Q>& mat) {
        ar(make_size_tag(static_cast<size_type>(C)));
        for (glm::length_t i = 0; i < C; ++i) {
            ar(mat[i]);
        }
    }

    template<class Archive, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void load(Archive& ar, glm::mat<C, R, T, Q>& mat) {
        size_type size;
        ar(make_size_tag(size));
        glm::length_t cols;
        if (C < size) {
            cols = C;
        } else {
            cols = static_cast<glm::length_t>(size);
        }
        for (glm::length_t i = 0; i < cols; ++i) {
            ar(mat[i]);
        }
    }

}