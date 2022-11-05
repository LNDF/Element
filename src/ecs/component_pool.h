#pragma once

#include "../utils/packed_map.h"
#include "../utils/uuid.h"
#include "../utils/padded_array_view.h"
#include "game_object.h"

#include <utility>

namespace engine {

    class component_pool_base {
        public:
            virtual ~component_pool_base() = default;
            virtual void game_object_destroyed(const uuid& o) = 0;
    };

    template<typename T>
    class component_pool : public component_pool_base, public packed_map<uuid, std::pair<game_object*, T>> {
        public:
            virtual ~component_pool() {

            }

            virtual void game_object_destroyed(const uuid& o) {
                this->erase(o);
            }
    };

} // namespace engine