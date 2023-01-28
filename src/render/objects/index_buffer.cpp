#include "index_buffer.h"

#include <core/engine.h>
#include <core/log.h>

#include <render/opengl/objects/index_buffer.h>

using namespace element;

index_buffer::~index_buffer() {}

index_buffer* index_buffer::create(uint32_t* indices, uint32_t count) {
    switch (engine::settings.renderer) {
        case opengl:
            return new opengl_index_buffer(indices, count);
        default:
            ELM_ERROR("Unsupported rendering API detected while trying to create index buffer.");
            return nullptr;
    }
}