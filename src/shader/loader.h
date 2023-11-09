#pragma once

#include <serialization/shader.h>
#include <resource/loader.h>

namespace element {
    namespace shader {
        
        ELM_MAKE_RESOURCE_LOADER(load_shader, shader_data)

    } // namespace shader
} // namespace element