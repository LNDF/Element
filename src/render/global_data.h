#pragma once

#include <graphics/vulkan.h>
#include <cinttypes>

namespace element {
    namespace render {
        namespace global_data {
            
            struct data {
                std::uint64_t time;
            };

            void init();
            void cleanup();
            vk::DescriptorSetLayout get_descriptorset_layout();
            vk::Buffer get_data_buffer();
            void record_sync(vk::CommandBuffer& cmd);

        } // namespace global_data
    } // namespace render
} // namespace element
