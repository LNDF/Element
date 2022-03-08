#ifndef __GLTEXTURE_H__
#define __GLTEXTURE_H__

#include "../../texture/texture.h"

class GLTexture : public Texture {
    private:
        u32 id;
    public:
        GLTexture();
        virtual ~GLTexture();
};

#endif