#pragma once

#include <graphics/vulkan_shader.h>
#include <utils/uuid.h>

namespace element {
    namespace render {
        vk::ShaderModule load_shader(const uuid& id);
    } // namespace render
} // namespace element
