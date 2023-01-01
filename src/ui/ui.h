#pragma once

#include <string>

#define ELM_WINDOW_CENTER -1

namespace element {
    namespace ui {
        
        extern void init_backend();
        extern void cleanup_backend();
        extern void start_ui();
        extern void stop_ui();
        extern int get_surface_width();
        extern int get_surface_height();
        extern void set_surface_size(int width, int height);
        extern void set_pointer_lock(bool lock);
        extern bool is_pointer_locked();
        extern void set_vsync(bool vsync);
        extern bool is_vsync_enabled();

    } // namespace window
} // namespace element
