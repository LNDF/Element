#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {

        class device_buffer {
            private:
                VmaAllocation buffer_alloc = nullptr;
                VmaAllocation staging_alloc = nullptr;
                vk::BufferUsageFlags usage;
                vk::Buffer buffer;
                vk::Buffer staging;
                std::uint32_t size;
                std::uint32_t capacity;
                bool upload_pending = false;
                bool staging_required = false;

                void create_staging();
                void reset_buffers();
                void destroy_buffers();
            public:
                device_buffer(std::uint32_t size, vk::BufferUsageFlags usage);
                ~device_buffer();

                device_buffer(const device_buffer& other) = delete;
                device_buffer(device_buffer&& other);
                device_buffer& operator=(const device_buffer& other) = delete;
                device_buffer& operator=(device_buffer&& other);

                void reserve(std::uint32_t capacity);
                void resize(std::uint32_t size);

                void set(const void* buffer);
                void record_upload(vk::CommandBuffer& cmd);
                void delete_staging();

                inline vk::Buffer get_buffer() const {return buffer;}
                inline bool uses_staging() const {return staging_required;}
        };

    } //namespace vulkan
} //namespace element