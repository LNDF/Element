#pragma once

#include <memory>
#include <render/objects/index_buffer.h>
#include <render/objects/vertex_buffer.h>

namespace element {
    
    class vertex_array {
        public:
            virtual ~vertex_array();
            virtual void bind() = 0;
            virtual void unbind() = 0;
            virtual uint32_t get_renderer_id() const = 0;
            virtual void set_index_buffer(std::shared_ptr<index_buffer> ibo) = 0;
            virtual void set_vertex_buffer(std::shared_ptr<vertex_buffer> vbo) = 0;

            static vertex_array* create();
            static vertex_array* create(std::shared_ptr<vertex_buffer> vbo, std::shared_ptr<index_buffer> ibo);

    };

} // namespace element
