#include "game_object.h"

#include <utility>
#include <core/log.h>

using namespace element;

game_object::game_object() : level(0) {}

game_object::game_object(const uuid& id, uuid parent, std::uint32_t level)
        : id(id), level(level), parent(parent) {}

game_object::game_object(game_object&& obj) 
        : id(obj.id), level(obj.level), children(std::move(obj.children)), parent(obj.parent), current_scene(obj.current_scene) {}

game_object& game_object::operator=(game_object&& other) {
    this->id = other.id;
    this->level = other.level;
    this->children = std::move(other.children);
    this->parent = other.parent;
    this->current_scene = other.current_scene;
    return *this;
}