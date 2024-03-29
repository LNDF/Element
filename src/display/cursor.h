#pragma once

#include <utility>

namespace element {
    namespace display {

        extern std::pair<int, int> get_cursor_position();
        extern void set_cursor_position(int x, int y);
        extern void grab_cursor();
        extern void release_cursor();

    } // namespace display
} // namespace element