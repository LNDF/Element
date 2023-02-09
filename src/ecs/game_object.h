#pragma once

#include <cstdint>
#include <vector>

#include <utils/uuid.h>

namespace element {

    class scene;

    class game_object {
        private:
            friend class scene;

            uuid id;
            std::uint32_t level;
            std::vector<game_object*> children;
            uuid parent;
            scene* current_scene = nullptr;

        public:
            game_object();

            game_object(const uuid& id, uuid parent, std::uint32_t level);
            game_object(const game_object& obj) = delete;
            game_object(game_object&& obj);

            game_object& operator=(const game_object& other) = delete;
            game_object& operator=(game_object&& other);

            inline const uuid& get_uuid() const {return id;}
            inline std::uint32_t get_level() const {return level;}
            inline const std::vector<game_object*>& get_children() {return children;}
            inline const uuid& get_parent() const {return parent;}
            inline const scene* get_scene() const {return current_scene;}
    };

} // namespace element