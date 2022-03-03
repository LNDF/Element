#ifndef __VERTEXARRAY_H__
#define __VERTEXARRAY_H__

#include "../common.h"
#include "gl_include.h"
#include "vertex_array_layout.h"
#include "vertex_buffer.h"

extern u32 boundVertexArray;

class VertexArray {
    private:
        u32 id;
    public:
        VertexArray();
        ~VertexArray();

        void bind() const;
        static void unbind();

        void addVertexBuffers(const VertexArrayLayout& layout, const VertexBuffer& buffer);

        inline u32 getId() const {return this->id;}
};

#endif