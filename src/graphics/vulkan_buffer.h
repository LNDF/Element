#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {

        class device_buffer_dynamic {
            private:
                VmaAllocation buffer_alloc = nullptr;
                VmaAllocation staging_alloc = nullptr;
                vk::Buffer buffer;
                vk::Buffer staging;
                std::uint32_t size;
                bool upload_pending = false;
            public:
                device_buffer_dynamic(std::uint32_t size, vk::BufferUsageFlags usage);
                ~device_buffer_dynamic();
                void set(const void* buffer);
                void record_upload(vk::CommandBuffer& cmd);

                inline vk::Buffer get_buffer() {return buffer;}
        };

    } //namespace vulkan
} //namespace element