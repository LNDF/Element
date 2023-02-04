#include "game_object.h"

#include <utility>

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
    all_objects[id] = this;
}

game_object::game_object(game_object&& obj) 
        : id(obj.id), level(obj.level), children(std::move<std::vector<game_object*>&>(obj.children)), parent(obj.parent), current_scene(obj.current_scene) {
    all_objects[obj.id] = this;
}

game_object* game_object::get_from_uuid(const uuid& id) {
    try {
        return all_objects.at(id);
    } catch (std::out_of_range e) {
        return nullptr;
    }
}