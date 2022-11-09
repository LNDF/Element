#include "scene.h"

using namespace engine;

scene::scene() {
    uuid r;
    root_object = &(*objects.try_emplace(r, r, nullptr, 0, this).first).second;
}

scene::~scene() {
    for (auto [s, p] : component_pools) {
        if (p) delete p;
    }
}

game_object* scene::get_game_object(const uuid& id) {
    if (!objects.contains(id)) return nullptr;
    return &objects[id];
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
    uuid c;
    return &(*objects.try_emplace(c, c, obj, obj->level + 1, this).first).second;
}