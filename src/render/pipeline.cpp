#include "pipeline.h"

#include <graphics/vulkan_shader.h>
#include <graphics/vulkan_descriptor.h>
#include <render/global_data.h>
#include <render/shader.h>
#include <render/scene_render.h>
#include <render/mesh.h>
#include <render/pipeline_manager.h>
#include <glm/glm.hpp>
#include <unordered_map>

using namespace element;

static std::unordered_map<uuid, render::pipeline> loaded_forward_pipelines;

static vk::DescriptorSetLayoutBinding create_descriptorset_layout_binding_from_resource(const render::shader_resource_layout& res, const vk::ShaderStageFlags& stage) {
    vk::DescriptorSetLayoutBinding binding;
    binding.binding = res.binding;
    binding.descriptorType = vk::DescriptorType::eUniformBuffer;
    binding.descriptorCount = 1;
    binding.stageFlags = stage;
    binding.pImmutableSamplers = nullptr;
    return binding;
}

static void populate_descriptorset_layouts(std::vector<vk::DescriptorSetLayout>& out, const render::pipeline_data& data) {
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> bindings;
    for (const auto& [resource, stages] : data.layouts) {
        if (resource.set == 0) continue;
        if (resource.set > bindings.size()) {
            bindings.resize(resource.set);
        }
        bindings[resource.set - 1].push_back(create_descriptorset_layout_binding_from_resource(resource, stages));
    }
    out.reserve(bindings.size() + 1);
    out.push_back(render::global_data::get_descriptorset_layout());
    for (const std::vector<vk::DescriptorSetLayoutBinding>& set : bindings) {
        if (set.empty()) {
            out.push_back(vulkan::create_empty_descriptorset_layout());
        } else {
            out.push_back(vulkan::create_descriptorset_layout_from_bindings(set));
        }
    }
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
    info.lineWidth = 1.0f;
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
    attributes.emplace_back(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::vertex, position)); //position
    attributes.emplace_back(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::vertex, normal)); //normal
    attributes.emplace_back(2, 0, vk::Format::eR32G32B32Sfloat, offsetof(render::vertex, tangent)); //tangent
    attributes.emplace_back(3, 0, vk::Format::eR32G32Sfloat, offsetof(render::vertex, tex_coords)); //tex coords
    //Per instance
    attributes.emplace_back(4, 0, vk::Format::eR32G32B32A32Sfloat, 0); //model matrix
    //Bindings
    bindings.reserve(2);
    bindings.emplace_back(0, sizeof(render::vertex), vk::VertexInputRate::eVertex); //vertex data
    bindings.emplace_back(1, sizeof(glm::mat4), vk::VertexInputRate::eInstance); //instance data
}

static render::pipeline create_forward_pipeline(const render::pipeline_data& data) {
    auto vert_spv = render::load_shader_spv(data.vert_id);
    auto frag_spv = render::load_shader_spv(data.frag_id);
    render::pipeline result;
    result.pipeline = nullptr;
    result.layout = nullptr;
    if (vert_spv.empty() || frag_spv.empty()) return result;
    vk::ShaderModule vert_mod = vulkan::create_shader_module(vert_spv);
    vk::ShaderModule frag_mod = vulkan::create_shader_module(frag_spv);
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
    input_assembly_info.primitiveRestartEnable = vk::False;
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
    //TODO: transparency support
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
    vk::PipelineLayoutCreateInfo pipeline_layout_info;
    pipeline_layout_info.flags = vk::PipelineLayoutCreateFlags();
    populate_descriptorset_layouts(result.descriptorset_layouts, data);
    pipeline_layout_info.pSetLayouts = result.descriptorset_layouts.data();
    pipeline_layout_info.setLayoutCount = result.descriptorset_layouts.size();
    vk::PushConstantRange push_constant_range;
    if (data.push_constants.first.size > 0) {
        push_constant_range.offset = 0;
        push_constant_range.size = data.push_constants.first.size;
        push_constant_range.stageFlags = data.push_constants.second;
        pipeline_layout_info.pPushConstantRanges = &push_constant_range;
        pipeline_layout_info.pushConstantRangeCount = 1;
    } else {
        pipeline_layout_info.pPushConstantRanges = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 0;
    }
    result.layout = vulkan::device.createPipelineLayout(pipeline_layout_info);
    info.layout = result.layout;
    result.descriptorset_layouts.erase(result.descriptorset_layouts.begin());
    info.renderPass = render::scene_renderer::forward_renderpass;
    info.subpass = 0;
    info.basePipelineHandle = nullptr;
    result.pipeline = vulkan::device.createGraphicsPipeline(nullptr, info).value;
    return result;
}

static void destroy_pipeline(const render::pipeline& pipeline) {
    if (pipeline.pipeline != nullptr) vulkan::device.destroyPipeline(pipeline.pipeline);
    if (pipeline.layout != nullptr) vulkan::device.destroyPipelineLayout(pipeline.layout);
    for (const vk::DescriptorSetLayout& layout : pipeline.descriptorset_layouts) {
        vulkan::device.destroyDescriptorSetLayout(layout);
    }
}

const render::pipeline* render::get_forward_pipeline(const uuid& id) {
    auto it = loaded_forward_pipelines.find(id);
    if (it != loaded_forward_pipelines.end()) {
        return &it->second;
    }
    const pipeline_data* data = get_pipeline_data(id);
    if (data == nullptr) return nullptr;
    pipeline p = create_forward_pipeline(*data);
    if (p.pipeline == nullptr) return nullptr;
    it = loaded_forward_pipelines.insert_or_assign(id, std::move(p)).first;
    return &it->second;
}

render::pipeline_data* render::get_pipeline_data(const uuid& id) {
    return pipeline_data_manager::get(id);
}

void render::destroy_pipeline(const uuid& id) {
    auto it = loaded_forward_pipelines.find(id);
    if (it != loaded_forward_pipelines.end()) {
        ::destroy_pipeline(it->second);
    }
    pipeline_data_manager::destroy(id);
}

void render::destroy_all_pipelines() {
    ELM_DEBUG("Destroying all loaded pipelines");
    for (auto& [id, pipeline] : loaded_forward_pipelines) {
        ::destroy_pipeline(pipeline);
    }
    pipeline_data_manager::destroy_all();
}

