#include "game_object.h"

#include <utility>
#include <core/log.h>

using namespace element;

std::unordered_map<uuid, game_object*> game_object::all_objects;

game_object::game_object() : level(0) {
    all_objects[id] = this;
}

game_object::~game_object() {
    all_objects.erase(id);
}

game_object::game_object(const uuid& id, game_object* parent, std::uint32_t level, scene* current_scene)
        : id(id), level(level), parent(parent), current_scene(current_scene) {
    if (all_objects.contains(id)) throw std::runtime_error("UUID is not available");
    all_objects[id] = this;
}

game_object::game_object(game_object&& obj) 
        : id(obj.id), level(obj.level), children(std::move(obj.children)), parent(obj.parent), current_scene(obj.current_scene) {
    all_objects[obj.id] = this;
}

game_object& game_object::operator=(game_object&& other) {
    this->id = other.id;
    this->level = other.level;
    this->children = std::move(other.children);
    this->parent = other.parent;
    this->current_scene = other.current_scene;
    all_objects[id] = this;
    return *this;
}

game_object* game_object::get_from_uuid(const uuid& id) {
    try {
        return all_objects.at(id);
    } catch (const std::out_of_range&) {
        ELM_DEBUG("Trying to get game object that doesn't exist.");
        return nullptr;
    }
}

uuid game_object::get_new_uuid() {
    uuid id;
    while (all_objects.contains(id)) id.regenerate();
    return id;
}