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

                device_buffer_dynamic(const device_buffer_dynamic& other) = delete;
                device_buffer_dynamic(device_buffer_dynamic&& other);
                device_buffer_dynamic& operator=(const device_buffer_dynamic& other) = delete;
                device_buffer_dynamic& operator=(device_buffer_dynamic&& other);

                void set(const void* buffer);
                void record_upload(vk::CommandBuffer& cmd);

                inline vk::Buffer get_buffer() {return buffer;}
        };

    } //namespace vulkan
} //namespace element