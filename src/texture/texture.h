#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "../common.h"

class Texture {
    public:
        virtual ~Texture();
        virtual u32 getId() const;
        virtual void bind();
        virtual void bind(u32 slot);
        virtual u32 getWidth() const;
        virtual u32 getheight() const;
};

#endif