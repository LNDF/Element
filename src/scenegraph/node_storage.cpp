#include "node_storage.h"

#include <scenegraph/node.h>
#include <ostream>

using namespace element;

std::uint32_t scenegraph::node_storage_base::next_storage_id = 0;

scenegraph::node_storage_base::node_storage_base() : storage_id(next_storage_id++) {}

scenegraph::node_storage_base::node_storage_base(scenegraph::scene* owner_scene) 
    : storage_id(next_storage_id++), owner_scene(owner_scene) {}

void scenegraph::node_storage_base::setup_node(node& n) {
    n.node_setup();
}

void scenegraph::node_storage_base::cleanup_node(node& n) {
    n.node_cleanup();
}

void scenegraph::node_storage_base::set_owner_scene_node(node& n) {
    n.set_owner_scene(this->owner_scene);
}