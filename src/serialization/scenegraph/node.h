#pragma once

#include <vector>
#include <string>
#include <scenegraph/node_ref.h>
#include <serialization/scenegraph/node_ref.h>
#include <serialization/vector.h>
#include <serialization/string.h>

namespace cereal {
    
    template<typename Archive>
    void save(Archive& ar, const element::scenegraph::node& node) {
        ar(make_nvp("id", node.get_id()));
        ar(make_nvp("name", node.get_name()));
        ar(make_nvp("parent", node.get_parent()));
        ar(make_nvp("children", node.get_children()));
    }

    template<typename Archive>
    void load(Archive& ar, element::scenegraph::node& node) {
        uuid id;
        std::string name;
        element::scenegraph::node_ref parent;
        std::vector<element::scenegraph::node_ref> children;
        ar(make_nvp("id", id));
        ar(make_nvp("name", name));
        ar(make_nvp("parent", parent));
        ar(make_nvp("children", children));
        node.set_id(id);
        node.set_name(std::move(name));
        node.set_parent(parent);
        node.set_children(std::move(children));
    }

} // namespace cereal
