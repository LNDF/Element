#pragma once

#include <render/objects/index_buffer.h>

namespace element {
    
    class opengl_index_buffer : public index_buffer {
        private:
            uint32_t renderer_id;
            uint32_t count;
        public:
            opengl_index_buffer(uint32_t* indices, uint32_t count);
            ~opengl_index_buffer() final override;

            void bind() final override;
            void unbind() final override;
            uint32_t get_renderer_id() const final override;
            uint32_t get_count() const final override;
    };

} // namespace element
