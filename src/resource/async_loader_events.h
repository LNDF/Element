#pragma once

#include <utils/uuid.h>

namespace element {
    namespace events {
        
        struct resource_start_loading {
            uuid id;
        };

        struct resource_end_loading {
            uuid id;
        };

    } // namespace revents
} // namespace element
