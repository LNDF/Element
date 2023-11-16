#include "material.h"

using namespace element;

template<typename T>
static void write_to_buffer(const T& t, render::material_buffer& buffer, std::uint32_t offset) {
    T* dest = (T*) &buffer.data[offset];
    *dest = t;
    buffer.needs_sync = true;
}

template<typename T>
static const T* read_from_buffer(const render::material_buffer& buffer, std::uint32_t offset) {
    return (const T*) &buffer[offset];
}

