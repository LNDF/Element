#include "game_object.h"

#include <utility>

using namespace engine;

game_object::game_object() : level(0) {}

game_object::game_object(const uuid& id, game_object* parent, std::uint32_t level, scene* current_scene)
    : id(id), level(level), parent(parent), current_scene(current_scene) {}

game_object::game_object(game_object&& obj) 
    : id(obj.id), level(obj.level), children(std::move<std::vector<game_object*>&>(obj.children)), parent(obj.parent), current_scene(obj.current_scene), signature(obj.signature) {}