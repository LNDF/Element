#pragma once

#include <ecs/game_object.h>

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>

namespace cereal {

    // template<>
    // struct LoadAndConstruct<element::game_object> {

    //     template<class Archive>
    //     static void load_and_construct(Archive& ar, construct<element::game_object>& construct) {
    //         element::uuid id, parent;
    //         std::uint32_t level;
    //         element::packed_set<element::uuid> children;
    //         ar(cereal::make_nvp("uuid", id),
    //            cereal::make_nvp("level",    level),
    //            cereal::make_nvp("children", children),
    //            cereal::make_nvp("parent",   parent));
    //         construct(std::move(id), std::move(level), std::move(children), std::move(parent));
    //     }

    // };

    template<class Archive>
    inline void save(Archive& ar, const element::game_object& object) {
        ar(cereal::make_nvp("uuid",     object.get_uuid()),
           cereal::make_nvp("level",    object.get_level()),
           cereal::make_nvp("children", object.get_children()),
           cereal::make_nvp("parent",   object.get_parent()));
    }

    template<class Archive>
    inline void load(Archive& ar, element::game_object& object) {
        element::uuid id, parent;
        std::uint32_t level;
        element::packed_set<element::uuid> children;
        ar(cereal::make_nvp("uuid", id),
           cereal::make_nvp("level",    level),
           cereal::make_nvp("children", children),
           cereal::make_nvp("parent",   parent));
        object.load(std::move(id), std::move(parent), std::move(children), std::move(level));
    }
    
} // namespace cereal