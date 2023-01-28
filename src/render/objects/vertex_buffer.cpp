#include "vertex_buffer.h"

#include <core/engine.h>
#include <core/log.h>

#include <render/opengl/objects/vertex_buffer.h>

using namespace element;

vertex_buffer::~vertex_buffer() {}

vertex_buffer* vertex_buffer::create(void* data, uint32_t size) {
    switch (engine::settings.renderer) {
        case opengl:
            return new opengl_vertex_buffer(data, size);
        default:
            ELM_ERROR("Unsupported rendering API detected while trying to create vertex buffer.");
            return nullptr;
    }
}

uint32_t vertex_buffer_layout_element::size() const {
    switch (this->type) {
        case boolean:
            return 1;
        case int1: case float1:
            return 4;
        case int2: case float2:
            return 4 * 2;
        case int3: case float3:
            return 4 * 3;
        case int4: case float4:
            return 4 * 4;
        case mat2:
            return 4 * 2 * 2;
        case mat3:
            return 4 * 3 * 3;
        case mat4:
            return 4 * 4 * 4;
        default:
            return 0;
    }
}

uint32_t vertex_buffer_layout_element::count() const { //TODO: matrix
    switch (this->type) {
        case boolean: case int1: case float1:
            return 1;
        case int2: case float2:
            return 2;
        case int3: case float3:
            return 3;
        case int4: case float4:
            return 4;
        default:
            return 0;
    }
}

vertex_buffer_layout_element::vertex_buffer_layout_element(vertex_buffer_layout_element_type type, bool normalized)
        : type(type), normalized(normalized) {}

vertex_buffer_layout::vertex_buffer_layout(std::initializer_list<vertex_buffer_layout_element> elements)
        : elements{elements} {
    this->stride = 0;
    for (vertex_buffer_layout_element& element : this->elements) {
        element.offset = stride;
        stride += element.size();
    }
}