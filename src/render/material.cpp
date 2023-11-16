#include "material.h"

using namespace element;

template<typename T>
static void write_to_buffer(const T& t, render::material_buffer& buffer, std::uint32_t offset) {
    T* dest = (T*) &buffer.data[offset];
    *dest = t;
    buffer.needs_sync = true;
}

template<typename T>
static void read_from_buffer(T& t, const render::material_buffer& buffer, std::uint32_t offset) {
    const T* ptr = (const T*) &buffer[offset];
    t = *ptr;
}

template<typename T, glm::length_t L, glm::qualifier Q>
static void write_to_buffer<glm::vec<L, T, Q>>(const glm::vec<L, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset) {
    for (glm::length_t i = 0; i < L; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += sizeof(T);
    }
}

template<typename T, glm::length_t L, glm::qualifier Q>
static void read_from_buffer<glm::vec<L, T, Q>>(glm::vec<L, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset) {
    for (glm::length_t i = 0; i < L; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += sizeof(T);
    }
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
static void write_to_buffer<glm::mat<C, R, T, Q>>(const glm::mat<C, R, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (glm::length_t i = 0; i < C; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
static void read_from_buffer<glm::mat<C, R, T, Q>>(glm::mat<C, R, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset) {
    for (glm::length_t i = 0; i < C; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T>
static void write_array_to_buffer(const T* t, std::uint32_t size, render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (std::uint32_t i = 0; i < size; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T>
static void read_array_from_buffer(T* t, std::uint32_t size, render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (std::uint32_t i = 0; i < size; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

std::pair<render::material_buffer*, render::shader_block_member*> render::material::get_buffer_and_layout(const std::string& name) {
    auto prop_it = properties.find(name);
    if (prop_it == properties.end()) return std::make_pair(nullptr, nullptr);
    std::pair<material_buffer*, render::shader_block_member*> result;
    if (prop_it->second.buffer_index == material::push_constants_index) {
        result.first = &push_constants_buffer;
    } else {
        result.first = &uniform_buffers[prop_it->second.buffer_index];
    }
    result.second = prop_it->second.property;
    return result;
}
