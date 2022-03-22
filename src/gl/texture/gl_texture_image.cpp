#include "gl_texture_image.h"
#include "../../image.h"
#include "../gl_include.h"
#include <stdexcept>

void GLTextureImage::init(const u8* data, u32 width, u32 height) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

GLTextureImage::GLTextureImage(const u8* data, u32 width, u32 height) {
    this->init(data, width, height);
}

GLTextureImage::GLTextureImage(const u8* data, u32 len) {
    u32 width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    u8* img = stbi_load_from_memory(data, len, (int*) &width, (int*) &height, (int*) &channels, 4);
    if (img == NULL) {
        throw std::runtime_error("Errol loading image from memory");
    }
    this->init(img, width, height);
    stbi_image_free(img);
}

GLTextureImage::GLTextureImage(const std::string& path) {
    u32 width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    u8* img = stbi_load(path.c_str(), (int*) &width, (int*) &height, (int*) &channels, 4);
    if (img == NULL) {
        throw std::runtime_error("Error loading image" + path);
    }
    this->init(img, width, height);
    stbi_image_free(img);
}

void GLTextureImage::setFiltering(TextureFiltering filtering) {
    this->bind();

}