#ifndef __GLTEXTUREIMAGE_H__
#define __GLTEXTUREIMAGE_H__

#include <string>
#include "gl_texture.h"
#include "../../texture/texture_image.h"

class GLTextureImage : public GLTexture, TextureImage {
    private:
        void init(const u8* data, u32 width, u32 height);
    public:
        GLTextureImage(const std::string& path);
        GLTextureImage(const u8* data, u32 len);
        GLTextureImage(const u8* data, u32 width, u32 height);
        void setFiltering(TextureFiltering filtering);
};

#endif