#pragma once

#include <string>

namespace element {
    namespace display {

        enum class window_mode {
            windowed,
            fullscreen,
            borderless
        };
        
        extern void open_window();
        extern void close_window();
        extern void set_window_title(const std::string& title);
        extern void set_window_size(int width, int height);
        extern void set_window_position(int x, int y);
        extern void set_window_mode(window_mode mode);
        extern void set_window_resizable(bool resizable);

    } // namespace display
} // namespace element