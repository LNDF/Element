#pragma once

#include <scenegraph/nodes/camera_node.h>
#include <serialization/scenegraph/node.h>

#include <cereal/cereal.hpp>

namespace cereal {
    template<typename Archive>
    void save(Archive& ar, const element::scenegraph::camera_node& camera_node) {
        ar(make_nvp("node", cereal::base_class<element::scenegraph::node>(&camera_node)));
        ar(make_nvp("near_plane", camera_node.get_near_plane()));
        ar(make_nvp("far_plane", camera_node.get_far_plane()));
        ar(make_nvp("fov", camera_node.get_fov()));
    }

    template<typename Archive>
    void load(Archive& ar, element::scenegraph::camera_node& camera_node) {
        ar(make_nvp("node", cereal::base_class<element::scenegraph::node>(&camera_node)));
        float near_plane;
        float far_plane;
        float fov;
        ar(make_nvp("near_plane", near_plane));
        ar(make_nvp("far_plane", far_plane));
        ar(make_nvp("fov", fov));
        camera_node.set_near_plane(near_plane);
        camera_node.set_far_plane(far_plane);
        camera_node.set_fov(fov);

    }
} // namespace cereal

ELM_REGISTER_NODE_SERIALIZATION(element::scenegraph::camera_node)