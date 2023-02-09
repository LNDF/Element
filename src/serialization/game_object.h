#pragma once

#include <ecs/game_object.h>

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>

namespace cereal {

    template<>
    struct LoadAndConstruct<element::game_object> {

        template<class Archive>
        static void load_and_construct(Archive& ar, construct<element::game_object>& construct) {
            //TODO: load code
        }

    };

    template<class Archive>
    inline void save(Archive& ar, const element::game_object& object) {
        //TODO: save code
    }

    template<class Archive>
    inline void load(Archive& ar, element::game_object& object) {}
    
} // namespace cereal
