#pragma once

#include <scenegraph/node_ref.h>

namespace element {
    namespace scenegraph {
        class mesh_node;

        using mesh_node_ref = node_ref_derived<mesh_node>;
    } // namespace scenegraph
} // namespace element