#include "index_buffer.h"

#include <glad/glad.h>

using namespace element;

opengl_index_buffer::opengl_index_buffer(uint32_t* indices, uint32_t count) {
    glGenBuffers(1, &this->renderer_id);
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

opengl_index_buffer::~opengl_index_buffer() {
    glDeleteBuffers(1, &this->renderer_id);
}

void opengl_index_buffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->renderer_id);
}

void opengl_index_buffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t opengl_index_buffer::get_renderer_id() const {
    return this->renderer_id;
}

uint32_t opengl_index_buffer::get_count() const {
    return this->count;
}