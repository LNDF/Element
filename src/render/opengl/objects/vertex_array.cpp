#include "vertex_array.h"

#include <glad/glad.h>

using namespace element;

opengl_vertex_array::opengl_vertex_array() {
    glGenVertexArrays(1, &this->renderer_id);
}

opengl_vertex_array::opengl_vertex_array(std::shared_ptr<vertex_buffer> vbo, std::shared_ptr<index_buffer> ibo) {
    glGenVertexArrays(1, &this->renderer_id);
    this->set_vertex_buffer(vbo);
    this->set_index_buffer(ibo);
}

opengl_vertex_array::~opengl_vertex_array() {
    glDeleteVertexArrays(1, &this->renderer_id);
}

void opengl_vertex_array::bind() {
    glBindVertexArray(this->renderer_id);
}

void opengl_vertex_array::unbind() {
    glBindVertexArray(0);
}

uint32_t opengl_vertex_array::get_renderer_id() const {
    return this->renderer_id;
}

void opengl_vertex_array::set_index_buffer(std::shared_ptr<index_buffer> ibo) {
    this->bind();
    ibo->bind();
    this->ibo = ibo;
}

static uint32_t layout_element_type_to_gl_type(vertex_buffer_layout_element_type type) {
    switch (type) {
        case boolean:
            return GL_BOOL;
        case int1: case int2: case int3: case int4:
            return GL_INT;
        case float1: case float2: case float3: case float4: case mat2: case mat3: case mat4:
            return GL_FLOAT;
        default:
            return 0;
    }
}

void opengl_vertex_array::set_vertex_buffer(std::shared_ptr<vertex_buffer> vbo) {
    this->bind();
    vbo->bind();
    const vertex_buffer_layout& layout = vbo->get_layout();
    uint32_t index = 0;
    for (const vertex_buffer_layout_element& element : layout.elements) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, element.size(), layout_element_type_to_gl_type(element.type), element.normalized ? GL_TRUE : GL_FALSE, layout.stride, (const void*) element.offset);
    }
    this->vbo = vbo;
}