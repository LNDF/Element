#pragma once

#include <render/vulkan.h>
#include <render/vulkan_image_view.h>
#include <render/vulkan_image.h>

namespace element {
    namespace vulkan {
        
        template<vk::Format F, vk::ImageUsageFlagBits U, vk::ImageAspectFlagBits A>
        class render_target {
            private:
                image target;
                vk::ImageView view;

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
                static constexpr inline vk::Format format = F;
                static constexpr inline vk::ImageUsageFlagBits usage = U;
                static constexpr inline vk::ImageAspectFlagBits aspect = A;

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

        using color_attachment = render_target<vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits{VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT}, vk::ImageAspectFlagBits::eColor>;
        using depth_attachment = render_target<vk::Format::eD32Sfloat, vk::ImageUsageFlagBits{VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT}, vk::ImageAspectFlagBits::eDepth>;

    } // namespace vulkan 
} // namespace element
