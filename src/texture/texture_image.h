#ifndef __TEXTUREIMAGE_H__
#define __TEXTUREIMAGE_H__

#include "texture.h"
#include "texture_filtering.h"

class TextureImage : public virtual Texture {
    public:
        virtual ~TextureImage() = 0;
        virtual void setFiltering(TextureFiltering filtering)  = 0;
};

typedef ReferenceHandle<TextureImage> TextureImageHandle;

#endif