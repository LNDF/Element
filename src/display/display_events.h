#pragma once

#include <display/input.h>
#include <event/event.h>

namespace element {
    namespace events {
        
        struct mouse_down {
            display::mouse_button button;
        };

        struct mouse_up {
            display::mouse_button button;
        };

        struct key_down {
            display::key key;
        };

        struct key_up {
            display::key key;
        };

        struct mouse_move {
            int x;
            int y;
        };

        struct window_resize {
            int width;
            int height;
        };

    } // namespace events
} // namespace element