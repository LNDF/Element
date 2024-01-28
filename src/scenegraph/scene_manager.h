#pragma once

#include <scenegraph/scene.h>
#include <utils/uuid.h>

namespace element {
    namespace scenegraph {
        
        scene* load_scene(const uuid& id);
        scene* import_scene(const uuid& id, scene&& s);
        scene* get_scene(const uuid& id);
        void destroy_scene(const uuid& id);
        void destroy_all_scenes();

    } // namespace scenegraph
} // namespace element
