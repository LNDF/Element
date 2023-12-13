#pragma once

#include <scenegraph/node.h>
#include <utils/uuid.h>

namespace element {
    namespace scenegraph {
        
        class mesh_node : node {
            private:
                uuid mesh;
                uuid material;

                bool registered = false;

                void setup() final override;
                void cleanup() final override;
                void enable() final override;
                void disable() final override;
            public:
                using node::node;

                inline const uuid& get_mesh() const {return mesh;}
                inline const uuid& get_material() const {return material;}

                void set_mesh(const uuid& mesh);
                void set_material(const uuid& material);
        };

    } // namespace scenegraph
} // namespace element
