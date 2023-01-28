#include "vertex_buffer.h"

#include <glad/glad.h>

using namespace element;

opengl_vertex_buffer::opengl_vertex_buffer(void* data, uint32_t size) {
    glGenBuffers(1, &this->renderer_id);
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

opengl_vertex_buffer::~opengl_vertex_buffer() {
    glDeleteBuffers(1, &this->renderer_id);
}

void opengl_vertex_buffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
}

void opengl_vertex_buffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint32_t opengl_vertex_buffer::get_renderer_id() const {
    return this->renderer_id;
}