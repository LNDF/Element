#pragma once

namespace element {
    namespace events {
        
        struct mouse_move {
            int x, y;
        };

        struct key_event {
            enum key_event_type {
                KEY_DOWN, KEY_UP, KEY_HOLD
            };
            key_event_type type;
            int keycode;
        };        

    } // namespace events
} // namespace element
