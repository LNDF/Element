#include "index_buffer.h"

IndexBuffer::IndexBuffer(const u32* data, const u32 count) {
    this->count = count;
    glGenBuffers(1, &this->id);
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &this->id);
}

void IndexBuffer::draw() const {
    this->bind();
    glDrawElements(GL_TRIANGLES, this->count, GL_UNSIGNED_INT, nullptr);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}