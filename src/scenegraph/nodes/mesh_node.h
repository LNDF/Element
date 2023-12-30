#pragma once

#include <scenegraph/node.h>
#include <utils/uuid.h>

namespace element {
    namespace render {
        class scene_render_data;
    } //namespcae render

    namespace scenegraph {
        
        class mesh_node : public node {
            private:
                uuid mesh = uuid::null();
                uuid material = uuid::null();

                render::scene_render_data* render_data = nullptr;
                bool registered = false;

                void render_register();
                void render_unregister();

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
