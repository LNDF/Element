#pragma once

#include <cstdint>
#include <vector>

#include "signature.h"
#include "../utils/uuid.h"

namespace engine {

    class scene;

    class game_object {

        public:
            game_object();

            inline const uuid& get_uuid() const {return id;}
            inline std::uint32_t get_level() const {return level;}
            inline const std::vector<game_object*>& get_children() {return children;}
            inline const game_object* get_parent() const {return parent;}
            inline const scene* get_scene() const {return current_scene;}
            inline signature_manager::signature get_signature() const {return signature;}
            
            game_object(const uuid& id, game_object* parent, std::uint32_t level, scene* current_scene);
            game_object(const game_object& obj) = delete;
            game_object(game_object&& obj);
        private:
            friend class scene;

            uuid id;
            std::uint32_t level;
            std::vector<game_object*> children;
            game_object* parent;
            scene* current_scene;
            signature_manager::signature signature;
    };

} // namespace engine