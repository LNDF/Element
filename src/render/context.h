#pragma once

namespace element {
    class rendering_context {
        public:
            virtual ~rendering_context();

            virtual void swap_buffers() = 0;
            virtual void make_current() = 0;
    };
} // namespace element
