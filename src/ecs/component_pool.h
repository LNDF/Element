#pragma once

#include <utils/packed_map.h>
#include <utils/uuid.h>
#include <utils/padded_array_view.h>
#include <ecs/game_object.h>
#include <ecs/scene.h>

#include <utility>

namespace element {

    class component_pool_base {
        private:
            
        public:
            virtual ~component_pool_base() = default;
            virtual void game_object_destroyed(const uuid& o) = 0;
            virtual void init(scene* current_scene) = 0;
    };

    template<typename T>
    class component_pool : public component_pool_base, public packed_map<uuid, T> {
        public:
            ~component_pool() final override {

            }

            void game_object_destroyed(const uuid& o) final override {
                this->erase(o);
            }

            void init(scene* current_scene) final override {
                for (auto& [id, data] : *this) {
                    data.init(current_scene, id);
                }
            }
    };

} // namespace element