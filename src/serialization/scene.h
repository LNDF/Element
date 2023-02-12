#pragma once

#include <ecs/scene.h>
#include <ecs/component_pool.h>
#include <serialization/packed_map.h>

#include <memory>
#include <typeindex>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/unordered_map.hpp>

namespace cereal {

    template<class Archive, class H, class E, class A>
    inline void save(Archive& ar, const element::packed_map<std::type_index, std::unique_ptr<element::component_pool_base>, H, E, A>& map) {
        ar(make_size_tag(static_cast<size_type>(map.size())));
        for (const auto& i : map) {
            ar(i.second);
        }
    }

    template<class Archive, class H, class E, class A>
    inline void load(Archive& ar, element::packed_map<std::type_index, std::unique_ptr<element::component_pool_base>, H, E, A>& map) {
        size_type size;
        ar(make_size_tag(size));
        map.clear();
        map.reserve(static_cast<std::size_t>(size));
        for (size_t i = 0; i < size; ++i) {
            std::unique_ptr<element::component_pool_base> value;
            ar(value);
            map.emplace(std::move(value->get_type_index()), std::move(value));
        }
    }

    template<>
    struct LoadAndConstruct<element::scene> {

        template<class Archive>
        static void load_and_construct(Archive& ar, construct<element::scene>& construct) {
            element::uuid id;
            std::unordered_map<element::uuid, element::game_object> objects;
            element::packed_map<std::type_index, std::unique_ptr<element::component_pool_base>> pools;
            ar(cereal::make_nvp("uuid",         id),
               cereal::make_nvp("game_objects", objects),
               cereal::make_nvp("components",   pools));
            construct(std::move(id), std::move(objects), std::move(pools));
        }

    };

    template<class Archive>
    inline void save(Archive& ar, const element::scene& scene) {
        ar(cereal::make_nvp("uuid",         scene.get_uuid()),
           cereal::make_nvp("game_objects", scene.get_game_objects()),
           cereal::make_nvp("components",   scene.get_component_pools()));
    }

    template<class Archive>
    inline void load(Archive& ar, element::scene& scene) {}
    
} // namespace cereal
