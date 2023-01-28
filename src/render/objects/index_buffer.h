#pragma once

#include <cinttypes>

namespace element {
    
    class index_buffer {
        public:
            virtual ~index_buffer();
            virtual void bind() = 0;
            virtual void unbind() = 0;
            virtual uint32_t get_renderer_id() const = 0;
            virtual uint32_t get_count() const = 0;

            static index_buffer* create(uint32_t* indices, uint32_t count);
    };

} // namespace element
