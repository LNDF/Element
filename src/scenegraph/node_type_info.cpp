#include "node_type_info.h"

using namespace element;

std::unordered_map<std::type_index, scenegraph::node_type_info>& __detail::__scenegraph_get_node_type_map() {
    static std::unordered_map<std::type_index, scenegraph::node_type_info> map;
    return map;
}

scenegraph::node_type_info::node_type_info(const std::string& name, storage_factory_type storage_factory)
    : name(name), storage_factory(storage_factory) {}

scenegraph::node_type_info::node_type_info(std::string&& name, storage_factory_type storage_factory)
    : name(std::move(name)), storage_factory(storage_factory) {}

const scenegraph::node_type_info& scenegraph::get_node_type_info(std::type_index type) {
    return __detail::__scenegraph_get_node_type_map().at(type);
}