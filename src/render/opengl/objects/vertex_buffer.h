#pragma once

#include <render/objects/vertex_buffer.h>

namespace element {
    
    class opengl_vertex_buffer : public vertex_buffer {
        private:
            uint32_t renderer_id;
        public:
            opengl_vertex_buffer(void* data, uint32_t size);
            ~opengl_vertex_buffer() final override;

            void bind() final override;
            void unbind() final override;
            uint32_t get_renderer_id() const final override;
    };

} // namespace element
