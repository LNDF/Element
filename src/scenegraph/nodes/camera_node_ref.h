#pragma once

#include <scenegraph/node_ref.h>

namespace element {
    namespace scenegraph {
        class camera_node;

        using camera_node_ref = node_ref_derived<camera_node>;
    } // namespace scenegraph
} // namespace element