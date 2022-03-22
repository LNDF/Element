#include "gl_texture.h"
#include "../gl_include.h"

u32 boundTextures[32] = {0};

GLTexture::GLTexture() {
    glGenTextures(1, &this->id);
}

GLTexture::~GLTexture() {
    for (u32 i = 0; i < 32; i++) {
        if (boundTextures[i] == this->id) boundTextures[i] = 0;
    }
    glDeleteTextures(1, &this->id);
}

u32 GLTexture::getId() const {
    return this->id;
}

void GLTexture::bind() {
    this->bind(0);
}

void GLTexture::bind(u32 slot) {
    if (boundTextures[slot] != this->id) {
        boundTextures[slot] = this->id;
        glBindTexture(GL_TEXTURE0 + slot, this->id);
    }
}

u32 GLTexture::getWidth() const {
    return this->width;
}

u32 GLTexture::getheight() const {
    return this->height;
}