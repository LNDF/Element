#include "scene.h"

using namespace element;

std::unordered_map<uuid, scene*> scene::all_scenes;

scene::scene() {
    while (all_scenes.contains(id)) id.regenerate();
    all_scenes[id] = this;
    uuid r;
    while (this->objects.contains(r)) r.regenerate();
    root_object = &(*objects.try_emplace(r, r, r, 0).first).second;
}

scene::~scene() {
    all_scenes.erase(id);
}

scene::scene(uuid root_uuid, std::unordered_map<uuid, game_object>&& objects, packed_map<std::type_index, std::unique_ptr<component_pool_base>>&& components)
        : objects(std::move(objects)), component_pools(std::move(components)) {
        root_object = this->get_game_object(root_uuid);
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

void scene::remove_game_object(game_object* object, bool top_level) {
    if (object == this->root_object) return;
    for (const uuid& c : object->children) remove_game_object(this->get_game_object(c));
    for (auto& [s, p] : component_pools) {
        if (p) p->game_object_destroyed(object->get_uuid());
    }
    if (top_level) {
        this->get_game_object(object->get_uuid())->children.erase(object->get_uuid());
    }
    objects.erase(object->get_uuid());
}

game_object* scene::create_child(game_object* obj) {
    uuid c;
    while (this->objects.contains(c)) c.regenerate();
    game_object* o = &(*objects.try_emplace(c, c, obj->get_uuid(), obj->level + 1).first).second;
    obj->children.insert(o->get_uuid());
    if (this->initialized) o->current_scene = this;
    return o;
}

void scene::init() {
    if (this->initialized) return;
    this->initialized = true;
    for (auto& [id, object] : this->objects) {
        object.current_scene = this;
    }
    for (auto& [type, pool] : this->component_pools) {
        pool->init(this);
    }

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