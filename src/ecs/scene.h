#pragma once

#include <array>
#include <unordered_map>
#include <utility>
#include <typeindex>

#include <ecs/game_object.h>
#include <ecs/component_pool.h>
#include <ecs/view.h>
#include <utils/uuid.h>
#include <utils/packed_map.h>



namespace element {

    class scene {
        public:
            scene();
            ~scene();
            game_object* new_game_object();
            void destroy_game_object();

            template<typename T>
            component_pool<T>* get_component_pool() {
                std::type_index t(typeid(T));
                if (!component_pools.contains(t)) {
                    component_pool<T>* c = new component_pool<T>();
                    component_pools.try_emplace(t, c);
                    return c;
                }
                return reinterpret_cast<component_pool<T>*>(component_pools.at(t));
            }

            template<typename T>
            void remove_component(game_object* object) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->erase(object->id);
            }

            template<typename T>
            void add_component(game_object* object, T&& comp) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->try_emplace(object->id, object, std::move(comp));
            }

            template<typename T>
            void add_component(game_object* object, const T& comp) {
                component_pool<T>* pool = get_component_pool<T>();
                pool->try_emplace(object->id, object, comp);
            }

            template<typename T>
            T* get_component(const game_object* object) {
                component_pool<T>* pool = get_component_pool<T>();
                if (!pool->contains(object->id)) return nullptr;
                return &(*pool)[object->id].second;
            }

            template<typename T>
            bool has_component(game_object* object) {
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
            packed_map<std::type_index, component_pool_base*> component_pools;
            game_object* root_object;

    };
    
} // namespace element