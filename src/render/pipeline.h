#pragma once

#include <utils/uuid.h>
#include <graphics/vulkan.h>
#include <render/shader.h>
#include <vector>
#include <utility>
#include <optional>

namespace element {
    namespace render {

        enum culling_face {
            counter_clockwise,
            clockwise
        };

        struct pipeline_data {
            uuid vert_id;
            uuid frag_id;
            std::vector<std::pair<shader_resource_layout, vk::ShaderStageFlags>> layouts;
            std::pair<shader_resource_layout, vk::ShaderStageFlags> push_constants;
            bool backface_culling;
            culling_face frontface;
            bool transparent;
        };

        struct pipeline {
            vk::Pipeline pipeline;
            vk::PipelineLayout layout;
            std::vector<vk::DescriptorSetLayout> descriptorset_layouts;
        };

        const pipeline* get_forward_pipeline(const uuid& id);
        pipeline_data* get_pipeline_data(const uuid& id);
        void destroy_pipeline(const uuid& id);
        void destroy_all_pipelines();

    } // namespace render
} // namespace element
