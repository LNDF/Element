#include "scene.h"

using namespace element;

std::unordered_map<uuid, scene*> scene::all_scenes;

scene::scene() {
    while (all_scenes.contains(id)) id.regenerate();
    all_scenes[id] = this;
    uuid r = game_object::get_new_uuid();
    root_object = &(*objects.try_emplace(r, r, nullptr, 0, this).first).second;
}

scene::~scene() {
    for (auto [s, p] : component_pools) {
        if (p) delete p;
    }
    all_scenes.erase(id);
}

game_object* scene::get_game_object(const uuid& id) {
    try {
        return &objects[id];
    } catch (const std::out_of_range&) {
        return nullptr;
    }
}

bool scene::has_game_object(const uuid& id) {
    return objects.contains(id);
}

void scene::remove_game_object(game_object* object) {
    for (game_object* c : object->children) remove_game_object(c);
    for (auto [s, p] : component_pools) {
        if (p) p->game_object_destroyed(object->id);
    }
    objects.erase(object->id);
}

game_object* scene::create_child(game_object* obj) {
    uuid c = game_object::get_new_uuid();
    return &(*objects.try_emplace(c, c, obj, obj->level + 1, this).first).second;
}

scene* scene::get_from_uuid(const uuid& id) {
    try {
        return all_scenes.at(id);
    } catch (const std::out_of_range&) {
        return nullptr;
    }
}

uuid scene::get_new_uuid() {
    uuid id;
    while (all_scenes.contains(id)) id.regenerate();
    return id;
}