#include "pipeline.h"

#include <graphics/vulkan_shader.h>
#include <render/shader_loader.h>
#include <render/mesh.h>
#include <glm/glm.hpp>

using namespace element;

static vk::PipelineLayout create_pipeline_layout(const render::shader_layout& layout) {

}

static vk::PipelineDepthStencilStateCreateInfo create_pipeline_depth_stencil_state() {
    vk::PipelineDepthStencilStateCreateInfo info;
    info.flags = vk::PipelineDepthStencilStateCreateFlags();
    info.depthTestEnable = vk::True;
    info.depthWriteEnable = vk::True;
    info.depthCompareOp = vk::CompareOp::eLess;
    info.depthBoundsTestEnable = vk::False;
    info.stencilTestEnable = vk::False;
    return info;
}

static vk::PipelineRasterizationStateCreateInfo create_pipeline_rasterization_state(bool backface_culling, render::culling_face frontface) {
    vk::PipelineRasterizationStateCreateInfo info;
    info.flags = vk::PipelineRasterizationStateCreateFlags();
    info.depthClampEnable = vk::False;
    info.rasterizerDiscardEnable = vk::False;
    info.polygonMode = vk::PolygonMode::eFill;
    info.cullMode = backface_culling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone;
    switch (frontface) {
        case render::culling_face::clockwise:
            info.frontFace = vk::FrontFace::eClockwise;
            break;
        case render::culling_face::counter_clockwise:
            info.frontFace = vk::FrontFace::eCounterClockwise;
            break;
        default:
            break;
    }
    info.depthBiasEnable = vk::False;
    return info;
}

static vk::PipelineShaderStageCreateInfo create_pipeline_shader_stage(const vk::ShaderModule& module, vk::ShaderStageFlagBits stage) {
    vk::PipelineShaderStageCreateInfo info;
    info.flags = vk::PipelineShaderStageCreateFlags();
    info.stage = stage;
    info.module = module;
    info.pName = "main";
    return info;
}

static void create_pipeline_vertex_input_state(std::vector<vk::VertexInputAttributeDescription>& attributes, std::vector<vk::VertexInputBindingDescription>& bindings) {
    //Per vertex
    attributes.reserve(5);
    attributes.emplace_back(0, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::vertex, position)); //position
    attributes.emplace_back(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::vertex, normal)); //normal
    attributes.emplace_back(2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(render::vertex, tangent)); //tangent
    attributes.emplace_back(3, 0, vk::Format::eR32G32Sfloat, offsetof(render::vertex, tex_coords)); //tex coords
    //Per instance
    attributes.emplace_back(4, 0, vk::Format::eR32G32B32A32Sfloat, 0); //model matrix
    //Bindings
    bindings.reserve(2);
    bindings.emplace_back(0, sizeof(render::vertex), vk::VertexInputRate::eVertex); //vertex data
    bindings.emplace_back(1, sizeof(glm::mat4), vk::VertexInputRate::eInstance); //instance data
}

static vk::Pipeline create_forward_pipeline(const render::pipeline_data& data) {
    auto vert_data = render::load_shader(data.vert_id);
    auto frag_data = render::load_shader(data.frag_id);
    if (vert_data == std::nullopt || frag_data == std::nullopt) return nullptr;
    vk::ShaderModule vert_mod = vulkan::create_shader_module(vert_data->spv);
    vk::ShaderModule frag_mod = vulkan::create_shader_module(frag_data->spv);
    vk::GraphicsPipelineCreateInfo info;
    info.flags = vk::PipelineCreateFlags();
    vk::PipelineShaderStageCreateInfo stages[2] = {
        create_pipeline_shader_stage(vert_mod, vk::ShaderStageFlagBits::eVertex),
        create_pipeline_shader_stage(frag_mod, vk::ShaderStageFlagBits::eFragment)
    };
    info.pStages = stages;
    info.stageCount = 2;
    std::vector<vk::VertexInputAttributeDescription> vertex_input_attributes;
    std::vector<vk::VertexInputBindingDescription> vertex_input_bindings;
    create_pipeline_vertex_input_state(vertex_input_attributes, vertex_input_bindings);
    vk::PipelineVertexInputStateCreateInfo vertex_input_state;
    vertex_input_state.flags = vk::PipelineVertexInputStateCreateFlags();
    vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
    vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
    vertex_input_state.vertexAttributeDescriptionCount = vertex_input_attributes.size();
    vertex_input_state.vertexBindingDescriptionCount = vertex_input_bindings.size();
    info.pVertexInputState = &vertex_input_state;
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info;
    input_assembly_info.flags = vk::PipelineInputAssemblyStateCreateFlags();
    input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
    input_assembly_info.primitiveRestartEnable = vk::True;
    info.pInputAssemblyState = &input_assembly_info;
    vk::PipelineViewportStateCreateInfo viewport_state_info;
    viewport_state_info.flags = vk::PipelineViewportStateCreateFlags();
    viewport_state_info.pViewports = nullptr;
    viewport_state_info.pScissors = nullptr;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;
    info.pViewportState = &viewport_state_info;
    vk::PipelineRasterizationStateCreateInfo rasterization_state_info = create_pipeline_rasterization_state(data.backface_culling, data.frontface);
    info.pRasterizationState = & rasterization_state_info;
    vk::PipelineMultisampleStateCreateInfo multisample_state_info;
    multisample_state_info.flags = vk::PipelineMultisampleStateCreateFlags();
    multisample_state_info.sampleShadingEnable = vk::False;
    multisample_state_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
    info.pMultisampleState = &multisample_state_info;
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_info = create_pipeline_depth_stencil_state();
    info.pDepthStencilState = &depth_stencil_state_info;
    //TODO: transparent support
    vk::PipelineColorBlendStateCreateInfo color_blending_state_info;
    color_blending_state_info.flags = vk::PipelineColorBlendStateCreateFlags();
    vk::PipelineColorBlendAttachmentState color_blend_attachment;
    color_blend_attachment.blendEnable = vk::False;
    color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    color_blending_state_info.attachmentCount = 1;
    color_blending_state_info.pAttachments = &color_blend_attachment;
    info.pColorBlendState = &color_blending_state_info;
    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    dynamic_state_info.flags = vk::PipelineDynamicStateCreateFlags();
    vk::DynamicState dynamic_states[2] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamic_state_info.pDynamicStates = dynamic_states;
    dynamic_state_info.dynamicStateCount = 2;
    info.pDynamicState = &dynamic_state_info;
}