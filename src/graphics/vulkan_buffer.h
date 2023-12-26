#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {

        class upload_buffer {
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
                explicit upload_buffer(vk::BufferUsageFlags usage);
                upload_buffer(std::uint32_t size, vk::BufferUsageFlags usage);
                ~upload_buffer();

                upload_buffer(const upload_buffer& other) = delete;
                upload_buffer(upload_buffer&& other);
                upload_buffer& operator=(const upload_buffer& other) = delete;
                upload_buffer& operator=(upload_buffer&& other);

                void reserve(std::uint32_t capacity);
                void resize(std::uint32_t size);

                void set(const void* buffer);
                void record_upload(vk::CommandBuffer& cmd);
                void destroy_staging();

                inline vk::Buffer get_buffer() const {return buffer;}
                inline bool uses_staging() const {return staging_required;}
                inline std::uint32_t get_size() const {return size;}
        };

    } //namespace vulkan
} //namespace element