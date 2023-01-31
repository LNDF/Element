#pragma once

#include <render/objects/vertex_array.h>

namespace element {
    
    class opengl_vertex_array : public vertex_array {
        private:
            std::shared_ptr<index_buffer> ibo;
            std::shared_ptr<vertex_buffer> vbo;
            uint32_t renderer_id;
        public:
            opengl_vertex_array();
            opengl_vertex_array(std::shared_ptr<vertex_buffer> vbo, std::shared_ptr<index_buffer> ibo);
            ~opengl_vertex_array() final override;
            void bind() final override;
            void unbind() final override;
            uint32_t get_renderer_id() const final override;
            void set_index_buffer(std::shared_ptr<index_buffer> ibo) final override;
            void set_vertex_buffer(std::shared_ptr<vertex_buffer> vbo) final override;
    };

} // namespace element
