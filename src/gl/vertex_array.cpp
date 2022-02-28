#include "vertex_array.h"

u32 boundVertexArray = 0;

VertexArray::VertexArray() {
    glCreateVertexArrays(1, &this->id);
}

VertexArray::~VertexArray() {
    if (this->id == boundVertexArray) boundVertexArray = 0;
    glDeleteVertexArrays(1, &this->id);
}

void VertexArray::bind() const {
    if (this->id != boundVertexArray) {
        boundVertexArray = this->id;
        glBindVertexArray(this->id);
    }
}

void VertexArray::unbind() {
    if (boundVertexArray == 0) {
        boundVertexArray = 0;
        glBindVertexArray(0);
    }
}

void VertexArray::addVertexBuffers(const VertexArrayLayout& layout, const VertexBuffer& buffer) {
    this->bind();
    buffer.bind();
    const std::vector<VertexArrayLayoutElement>& elements = layout.getElements();
    u32 stride = layout.getStride();
    char* ptr = 0;
    for (u32 i = 0; i < elements.size(); i++) {
        const VertexArrayLayoutElement& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, stride, (const void*) ptr);
        ptr += element.size * element.count;
    }
}

u32 VertexArray::getId() const {
    return this->id;
}