#pragma once

#include <render/vulkan.h>

namespace element {
    namespace vulkan {
        
        class image {
            private:
                VmaAllocation image_alloc = nullptr;
                vk::Image vk_image;
                VmaAllocationCreateFlags alloc_flags;

                void destroy_image();
            public:
                explicit image(VmaAllocationCreateFlags alloc_flags);
                image(const vk::ImageCreateInfo& info, VmaAllocationCreateFlags alloc_flags);
                ~image();

                void create_image(const vk::ImageCreateInfo& info);

                inline vk::Image get_image() const {return vk_image;}
        };

        void transition_image_layout(vk::CommandBuffer& cmd, vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::ImageAspectFlags aspect_mask, std::uint32_t levels, std::uint32_t layers);

    } // namespace vulkan 
} // namespace element
