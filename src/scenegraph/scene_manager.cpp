#include "scene_manager.h"

#include <unordered_map>
#include <utility>
#include <core/log.h>
#include <core/fs.h>
#include <render/scene_data_manager.h>
#include <serialization/serializers.h>
#include <serialization/scenegraph.h>

static std::unordered_map<element::uuid, element::scenegraph::scene> scene_map;

using namespace element;

scenegraph::scene* scenegraph::load_scene(const uuid& id) {
    auto input = fs::get_resource(id);
    if (input->fail()) {
        ELM_WARN("Failed to load scene {}", id.str());
        return nullptr;
    }
    scene s;
    binary_deserializer deserialize = create_binary_deserializer(*input);
    deserialize(s);
    return import_scene(id, std::move(s));
    
}

scenegraph::scene* scenegraph::import_scene(const uuid& id, scene&& s) {
    render::create_scene_render_data(id);
    scene* inserted = &scene_map.insert(std::make_pair(id, std::move(s))).first->second;
    inserted->init_scene(id);
    return inserted;
}

scenegraph::scene* scenegraph::get_scene(const uuid& id) {
    auto it = scene_map.find(id);
    if (it == scene_map.end()) {
        return nullptr;
    }
    return &it->second;
}

void scenegraph::destroy_scene(const uuid& id) {
    scene_map.erase(id);
    render::destroy_scene_render_data(id);
}

void scenegraph::destroy_all_scenes() {
    scene_map.clear();
    render::destroy_all_scene_render_data();
}