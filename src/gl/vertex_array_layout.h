#ifndef __VERTEXARRAYLAYOUT_H__
#define __VERTEXARRAYLAYOUT_H__

#include "../common.h"
#include "gl_include.h"
#include <vector>

struct VertexArrayLayoutElement {
    u32 type;
    u32 count;
    u8 normalized;
    u32 size;
};

class VertexArrayLayout {
    private:
        std::vector<VertexArrayLayoutElement> elements;
        unsigned int stride;
    public:
        VertexArrayLayout();

        template <typename T>
        void push(u32 count);

        const std::vector<VertexArrayLayoutElement>& getElements() const;
        u32 getStride() const;
};

#endif