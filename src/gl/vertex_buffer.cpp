#include "vertex_buffer.h"

u32 boundVertexBuffer = 0;

VertexBuffer::VertexBuffer(const void* data, const u32 size) {
    this->size = size;
    glGenBuffers(1, &this->id);
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    if (boundVertexBuffer == this->id) boundVertexBuffer = 0;
    glDeleteBuffers(1, &this->id);
}

void VertexBuffer::bind() const {
    if (boundVertexBuffer != this->id) {
        boundVertexBuffer = this->id;
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
    }
}

void VertexBuffer::unbind() {
    if (boundVertexBuffer != 0) {
        boundVertexBuffer = 0;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}