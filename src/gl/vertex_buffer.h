#ifndef __VERTEXBUFFER_H__
#define __VERTEXBUFFER_H__

#include "../common.h"
#include "gl_include.h"

class VertexBuffer {
private:
    u32 id;
    u32 size;
public:
    VertexBuffer(const void* data, const u32 size);
    ~VertexBuffer();
    inline u32 getId() const {return this->id;}
    inline u32 getSize() const {return this->size;}
    void bind() const;
    static void unbind();
};

#endif