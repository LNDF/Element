#pragma once

#include <string>

namespace element {
    namespace window {
        
        extern void init_backend();
        extern void cleanup_backend();
        extern void create_window(int width, int height, int x, int y);
        extern void close_window();
        extern void set_title(const std::string& title);
        extern const std::string& get_title();
        extern int get_width();
        extern int get_height();
        extern int get_x_position();
        extern int get_y_position();
        extern void set_size(int width, int height);
        extern void move_window(int x, int y);
        extern void set_pointer_lock(bool lock);
        extern bool is_pointer_locked();
        extern void set_vsync(bool vsync);
        extern bool is_vsync_enabled();

    } // namespace window
} // namespace element
