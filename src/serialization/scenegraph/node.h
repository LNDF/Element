#pragma once

#include <vector>
#include <string>
#include <scenegraph/node_ref.h>
#include <scenegraph/node_storage.h>
#include <serialization/defs.h>
#include <serialization/scenegraph/node_ref.h>
#include <serialization/vector.h>
#include <serialization/string.h>

#define __ELM_SCENEGRAPH_MAKE_STORAGE_NAME(type) "storage_" #type

#define __ELM_SERIALIZATION_REGISTER_NODE_STORAGE(type) \
ELM_REGISTER_SERIALIZABLE_DERIVED_NAMED_TYPE(element::scenegraph::node_storage<type>, __ELM_SCENEGRAPH_MAKE_STORAGE_NAME(type)) \
ELM_REGISTER_DERIVED_BASE_SERIALIZATION_RELATION(element::scenegraph::node_storage<type>, element::scenegraph::node_storage_base)

#define ELM_REGISTER_NODE_SERIALIZATION(type) __ELM_SERIALIZATION_REGISTER_NODE_STORAGE(type) \
ELM_REGISTER_SERIALIZABLE_DERIVED_NAMED_TYPE(type, #type) \
ELM_REGISTER_DERIVED_BASE_SERIALIZATION_RELATION(type, element::scenegraph::node)

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
        element::uuid id;
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

__ELM_SERIALIZATION_REGISTER_NODE_STORAGE(element::scenegraph::node)