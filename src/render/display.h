#pragma once

#include "context.h"

namespace element {
    class display {
        public:
            virtual ~display();

            virtual int get_width() = 0;
            virtual int get_height() = 0;
            virtual void set_size(int width, int height) = 0;
            virtual rendering_context* get_rendering_context() = 0;
            virtual void update() = 0;

    };
    extern display* create_display();
    extern void update_all_displays();

} // namespace element
