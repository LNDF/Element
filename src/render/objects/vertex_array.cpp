#include "vertex_array.h"

#include <core/engine.h>
#include <render/opengl/objects/vertex_array.h>

using namespace element;

vertex_array::~vertex_array() {}

vertex_array* vertex_array::create() {
    switch (engine::settings.renderer) {
        case opengl:
            return new opengl_vertex_array();
        default:
            return nullptr;
    }
}

vertex_array* vertex_array::create(std::shared_ptr<vertex_buffer> vbo, std::shared_ptr<index_buffer> ibo) {
    switch (engine::settings.renderer) {
        case opengl:
            return new opengl_vertex_array(vbo, ibo);
        default:
            return nullptr;
    }
}