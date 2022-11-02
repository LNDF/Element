#pragma once

#include <array>
#include <unordered_map>
#include <utility>

#include "game_object.h"
#include "component_pool.h"
#include "signature.h"
#include "view.h"
#include "../utils/uuid.h"
#include "../utils/packed_map.h"



namespace engine {

    class scene {
        public:
            scene();
            ~scene();
            game_object* new_game_object();
            void destroy_game_object();

            template<typename T>
            component_pool<T>* get_component_pool() {
                std::uint32_t i = signature_manager::get_bit_position<T>();
                if (!component_pools[i]) {
                    component_pools[i] = new component_pool<T>();
                }
                return reinterpret_cast<component_pool<T>*>(component_pools[i]);
            }

            template<typename T>
            const component_pool<T>* get_component_pool() const {
                std::uint32_t i = signature_manager::get_bit_position<T>();
                if (!component_pools[i]) {
                    component_pools[i] = new component_pool<T>();
                }
                return reinterpret_cast<const component_pool<T>*>(component_pools[i]);
            }

            template<typename T>
            void remove_component(game_object* object) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->erase(object->id);
                object->signature.set(signature_manager::get_bit_position<T>(), false);
            }

            template<typename T>
            void add_component(game_object* object, T&& comp) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->try_emplace(object->id, object, std::move(comp));
                object->signature.set(signature_manager::get_bit_position<T>(), true);
            }

            template<typename T>
            void add_component(game_object* object, const T& comp) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->try_emplace(object->id, object, comp);
                object->signature.set(signature_manager::get_bit_position<T>(), true);
            }

            template<typename T>
            T* get_component(const game_object* object) {
                component_pool<T>* pool = get_component_pool<T>();
                if (!pool->contains(object->id)) return nullptr;
                return &(*pool)[object->id].second;
            }

            template<typename T>
            const T* get_component(const game_object* object) const {
                component_pool<T>* pool = get_component_pool<T>();
                return &pool[object->id].second;
            }

            template<typename T>
            bool has_component(game_object* object) const {
                component_pool<T>* pool = get_component_pool<T>();
                return pool.contains(object->id);
            }

            template<typename... T>
            scene_view<T...> view() {
                return scene_view<T...>(get_component_pool<T>()...);
            }

            inline game_object* create_game_object() {return create_child(root_object);}

            game_object* get_game_object(const uuid& uuid);
            bool has_game_object(const uuid& uuid);
            void remove_game_object(game_object* object);
            game_object* create_child(game_object* obj);


        private:
            friend class game_object;

            uuid id;
            std::unordered_map<uuid, game_object> objects;
            std::array<component_pool_base*, MAX_COMPONENT_COUNT> component_pools {};
            game_object* root_object;

    };
    
} // namespace engine