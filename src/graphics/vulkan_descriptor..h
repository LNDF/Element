#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {
        
        struct descriptor_set {
            vk::DescriptorSet set;
            vk::DescriptorPool pool;
        };

        vk::DescriptorSetLayout create_empty_descriptorset_layout();
        vk::DescriptorSetLayout create_descriptorset_layout_from_bindings(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);

        descriptor_set allocate_descriptorset(vk::DescriptorSetLayout& layout);
        void free_descriptorset(descriptor_set& set);
        void clean_descriptorsets();

    } // namespace vulkan
} // namespace element
