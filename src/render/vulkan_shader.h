#pragma once

#include <render/vulkan.h>
#include <vector>

namespace element {
    namespace vulkan {
        
        vk::ShaderModule create_shader_module(const std::vector<std::uint32_t>& spv);

    } // namespace vulkan
} // namespace element
