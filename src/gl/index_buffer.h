#ifndef __INDEXBUFFER_H__
#define __INDEXBUFFER_H__

#include "../common.h"
#include "gl_include.h"

class IndexBuffer {
private:
    u32 id;
    u32 count;
public:
    IndexBuffer(const u32* data, const u32 count);
    ~IndexBuffer();
    inline u32 getId() const {return this->id;}
    inline u32 getCount() const {return this->count;}
    void draw() const;
    void bind() const;
    static void unbind();
};


#endif