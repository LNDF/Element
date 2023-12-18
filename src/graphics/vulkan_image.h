#pragma once

#include <graphics/vulkan.h>

namespace element {
    namespace vulkan {
        
        class image {
            private:
                VmaAllocation image_alloc = nullptr;
                vk::Image vk_image;
                VmaAllocationCreateFlags alloc_flags;

                void destroy_image();
            public:
                image(VmaAllocationCreateFlags alloc_flags);
                image(const vk::ImageCreateInfo& info, VmaAllocationCreateFlags alloc_flags);
                ~image();

                void create_image(const vk::ImageCreateInfo& info);

                inline vk::Image get_image() const {return vk_image;}
        };

    } // namespace vulkan 
} // namespace element
