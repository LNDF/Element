#pragma once

#include <utils/uuid.h>

namespace element {
    namespace events {
        
        struct start_loading {
            uuid id;
        };

        struct end_loading {
            uuid id;
        };

    } // namespace revents
} // namespace element
