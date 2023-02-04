#pragma once

#include <istream>
#include <ostream>
#include <utils/uuid.h>

namespace element {
    extern std::ostream fs_get_data(const uuid& id);
    extern std::ostream fs_get_metadata(const uuid& id);
    extern std::istream fs_set_metadata(const uuid& id);
} // namespace element
