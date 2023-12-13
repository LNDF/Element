#pragma once

#include <scenegraph/nodes/mesh_node.h>
#include <serialization/scenegraph/node.h>

#include <cereal/cereal.hpp>

namespace cereal {
    template<typename Archive>
    void save(Archive& ar, const element::scenegraph::mesh_node& mesh_node) {
        ar(make_nvp("node", cereal::base_class<element::scenegraph::node>(&mesh_node)));
        ar(make_nvp("mesh", mesh_node.get_mesh()));
        ar(make_nvp("material", mesh_node.get_material()));
    }

    template<typename Archive>
    void load(Archive& ar, element::scenegraph::mesh_node& mesh_node) {
        ar(make_nvp("node", cereal::base_class<element::scenegraph::node>(&mesh_node)));
        element::uuid mesh;
        element::uuid material;
        ar(make_nvp("mesh", mesh));
        ar(make_nvp("material", material));
        mesh_node.set_mesh(mesh);
        mesh_node.set_material(material);

    }
} // namespace cereal

ELM_REGISTER_NODE_SERIALIZATION(element::scenegraph::mesh_node)