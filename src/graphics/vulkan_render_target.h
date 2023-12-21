#pragma once

#include <graphics/vulkan.h>
#include <graphics/vulkan_image_view.h>
#include <graphics/vulkan_image.h>

namespace element {
    namespace vulkan {
        
        template<vk::Format F, vk::ImageUsageFlags U, vk::ImageAspectFlags A>
        class render_target {
            private:
                image target;
                vk::ImageView view;
                vk::Format format;
                vk::ImageUsageFlags usage_flags;

                void create_target(std::uint32_t width, std::uint32_t height){
                    vk::ImageCreateInfo info;
                    info.flags = vk::ImageCreateFlags();
                    info.imageType = vk::ImageType::e2D;
                    info.extent.width = width;
                    info.extent.height = height;
                    info.extent.depth = 1;
                    info.mipLevels = 1;
                    info.arrayLayers = 1;
                    info.tiling = vk::ImageTiling::eOptimal;
                    info.initialLayout = vk::ImageLayout::eUndefined;
                    info.samples = vk::SampleCountFlagBits::e1;
                    info.format = F;
                    info.usage = U;
                    target.create_image(info);
                    view = create_image_view(target.get_image(), vk::ImageViewType::e2D, F, A, 1, 1);
                }
            public:
                render_target(std::uint32_t width, std::uint32_t height)
                    : target(VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT) {
                    create_target(width, height);
                }

                ~render_target() {
                    device.destroyImageView(view);
                }
                
                void resize(std::uint32_t width, std::uint32_t height) {
                    device.destroyImageView(view);
                    create_target(width, height);
                }

                inline vk::Image get_target() const {return target.get_image();}
                inline vk::ImageView get_view() const {return view;}
        };

        using color_attachment = render_target<vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor>;
        using depth_attachment = render_target<vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth>;

    } // namespace vulkan 
} // namespace element
