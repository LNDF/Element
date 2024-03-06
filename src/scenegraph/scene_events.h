#pragma once

#include <scenegraph/node_ref.h>
#include <utils/uuid.h>

namespace element {
    namespace events {
        
        struct before_node_added {
            scenegraph::node_ref parent;
            uuid id;
            std::uint32_t index;
        };

        struct after_node_added {
            scenegraph::node_ref parent;
            scenegraph::node_ref this_node;
        };

        struct before_node_deleted {
            scenegraph::node_ref parent;
            scenegraph::node_ref this_node;
            std::uint32_t index;
        };

        struct after_node_deleted {
            scenegraph::node_ref parent;
            uuid id;
        };

        struct before_node_moved {
            scenegraph::node_ref from_parent;
            scenegraph::node_ref to_parent;
            scenegraph::node_ref this_node;
            std::uint32_t dest_index;
        };

        struct after_node_moved {
            scenegraph::node_ref from_parent;
            scenegraph::node_ref to_parent;
            scenegraph::node_ref this_node;
        };

    } // namespace events
} // namespace element
