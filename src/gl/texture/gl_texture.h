#ifndef __GLTEXTURE_H__
#define __GLTEXTURE_H__

#include "../../texture/texture.h"

class GLTexture : public virtual Texture {
    private:
        u32 id;
    protected:
        u32 width;
        u32 height;
    public:
        GLTexture();
        ~GLTexture();
        
        u32 getId() const;
        void bind();
        void bind(u32 slot);
        u32 getWidth() const;
        u32 getheight() const;
};

#endif