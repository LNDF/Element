#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {
        
        vk::DescriptorSetLayout create_empty_descriptorset_layout();
        vk::DescriptorSetLayout create_descriptorset_layout_from_bindings(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

    } // namespace vulkan
} // namespace element
