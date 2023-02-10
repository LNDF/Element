#pragma once

#include <ecs/scene.h>

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>

namespace cereal {

    template<>
    struct LoadAndConstruct<element::scene> {

        template<class Archive>
        static void load_and_construct(Archive& ar, construct<element::scene>& construct) {
            //TODO: construct
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
