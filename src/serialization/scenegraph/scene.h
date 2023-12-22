#pragma once

#include <vector>
#include <string>
#include <scenegraph/scene.h>
#include <scenegraph/node_ref.h>
#include <scenegraph/nodes/camera_node_ref.h>
#include <serialization/scenegraph/node_ref.h>
#include <serialization/scenegraph/node.h>
#include <serialization/scenegraph/node_storage.h>
#include <serialization/vector.h>
#include <serialization/string.h>

namespace cereal {
    
    template<typename Archive, typename H, typename E, typename A>
    void save(Archive& ar, const std::unordered_map<std::type_index, std::unique_ptr<element::scenegraph::node_storage_base>, H, E, A>& map) {
        ar(make_size_tag(static_cast<size_type>(map.size())));
        for (const auto& [t, storage] : map) {
            ar(storage);
        }
    }

    template<typename Archive, typename H, typename E, typename A>
    void load(Archive& ar, std::unordered_map<std::type_index, std::unique_ptr<element::scenegraph::node_storage_base>, H, E, A>& map) {
        size_type size;
        ar(make_size_tag(size));
        map.clear();
        map.reserve(static_cast<std::size_t>(size));
        for (size_type i = 0; i < size; ++i) {
            std::unique_ptr<element::scenegraph::node_storage_base> value;
            ar(value);
            map.emplace(std::move(value->get_type_index()), std::move(value));
        }
    }

    template<typename Archive>
    void save(Archive& ar, const element::scenegraph::scene& scene) {
        ar(make_nvp("root_node", scene.get_root_node()));
        ar(make_nvp("default_camera", scene.get_default_camera()));
        ar(make_nvp("nodes", scene.get_node_storages()));
    }

    template<typename Archive>
    void load(Archive& ar, element::scenegraph::scene& scene) {
        element::scenegraph::node_ref root_node;
        element::scenegraph::camera_node_ref default_camera;
        std::unordered_map<std::type_index, std::unique_ptr<element::scenegraph::node_storage_base>> nodes;
        ar(make_nvp("root_node", root_node));
        ar(make_nvp("default_camera", default_camera));
        ar(make_nvp("nodes", nodes));
        scene.__set_root_node(root_node);
        scene.__set_node_storages(std::move(nodes));
        scene.set_default_camera(default_camera);
    }

} // namespace cereal
