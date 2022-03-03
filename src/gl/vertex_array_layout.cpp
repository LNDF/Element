#include "vertex_array_layout.h"

VertexArrayLayout::VertexArrayLayout() {
    this->stride = 0;
}

template<typename T>
void VertexArrayLayout::push(u32 count) {}

template<>
void VertexArrayLayout::push<float>(u32 count) {
    this->elements.push_back({GL_FLOAT, count, GL_FALSE, sizeof(GLfloat)});
    this->stride += sizeof(GLfloat) * count;
}

const std::vector<VertexArrayLayoutElement>& VertexArrayLayout::getElements() const {
    return this->elements;
}