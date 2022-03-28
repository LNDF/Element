#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "../common.h"
#include "../resource_handler.h"

class Texture {
    public:
        virtual ~Texture() = 0;
        virtual u32 getId() const  = 0;
        virtual void bind() = 0;
        virtual void bind(u32 slot) = 0;
        virtual u32 getWidth() const = 0;
        virtual u32 getheight() const = 0;
};

#endif