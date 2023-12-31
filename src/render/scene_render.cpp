#include "scene_render.h"

#include <graphics/vulkan_framebuffer.h>
#include <render/global_data.h>
#include <render/pipeline.h>
#include <render/material.h>
#include <render/mesh_manager.h>
#include <scenegraph/nodes/camera_node.h>
#include <scenegraph/scene_manager.h>

using namespace element;

vk::RenderPass render::scene_renderer::forward_renderpass;

void render::scene_renderer::create_forward_renderpass() {
    //Color attachment
    vk::AttachmentDescription color_attachment;
    color_attachment.flags = vk::AttachmentDescriptionFlags();
    color_attachment.format = vk::Format::eR8G8B8A8Unorm;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    //Depth attachment
    vk::AttachmentDescription depth_attachment;
    depth_attachment.flags = vk::AttachmentDescriptionFlags();
    depth_attachment.format = vk::Format::eD32Sfloat;
    depth_attachment.samples = vk::SampleCountFlagBits::e1;
    depth_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    depth_attachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depth_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depth_attachment.initialLayout = vk::ImageLayout::eUndefined;
    depth_attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    //Subpass's first attachment reference
    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    //Subpass's second attachment reference
    vk::AttachmentReference depth_attachment_reference;
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    //Subpass
    vk::SubpassDescription subpass;
    subpass.flags = vk::SubpassDescriptionFlags();
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.pColorAttachments = &color_attachment_reference;
    subpass.colorAttachmentCount = 1;
    subpass.pDepthStencilAttachment = &depth_attachment_reference;
    //Dependencies
    vk::SubpassDependency dependency;
    dependency.srcSubpass = vk::SubpassExternal;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    //Renderpass
    vk::AttachmentDescription attachments[] = {color_attachment, depth_attachment};
    vk::RenderPassCreateInfo renderpass_info;
    renderpass_info.flags = vk::RenderPassCreateFlags();
    renderpass_info.attachmentCount = 2;
    renderpass_info.pAttachments = attachments;
    renderpass_info.subpassCount = 1;
    renderpass_info.pSubpasses = &subpass;
    renderpass_info.dependencyCount = 1;
    renderpass_info.pDependencies = &dependency;
    forward_renderpass = vulkan::device.createRenderPass(renderpass_info);
}

void render::scene_renderer::init() {
    ELM_DEBUG("Initialize scene renderer");
    create_forward_renderpass();
}

void render::scene_renderer::cleanup() {
    ELM_DEBUG("Cleanning up scene renderer");
    vulkan::device.destroyRenderPass(forward_renderpass);
}

render::scene_renderer::scene_renderer(std::uint32_t width, std::uint32_t height)
    : width(width), height(height), cam_gpu_data(sizeof(camera_data), vk::BufferUsageFlagBits::eUniformBuffer), color(width, height),  depth(width, height) {
    create_framebuffer();
}

render::scene_renderer::~scene_renderer() {
    vulkan::device.destroyFramebuffer(framebuffer);
    if (scene_data != nullptr) vulkan::free_descriptorset(global_descriptorset);
}

void render::scene_renderer::create_framebuffer() {
    vk::ImageView attachments[] = {color.get_view(), depth.get_view()};
    framebuffer = vulkan::create_framebuffer(forward_renderpass, attachments, 2, width, height);
}

void render::scene_renderer::create_descriptorset() {
    global_descriptorset = vulkan::allocate_descriptorset(global_data::get_descriptorset_layout());
    //Global data write
    vk::WriteDescriptorSet write_sets[2];
    vk::WriteDescriptorSet& global_write_set = write_sets[0];
    vk::DescriptorBufferInfo global_buffer_info;
    global_buffer_info.buffer = global_data::get_data_buffer();
    global_buffer_info.offset = 0;
    global_buffer_info.range = sizeof(global_data::data);
    global_write_set.pBufferInfo = &global_buffer_info;
    global_write_set.dstBinding = 0;
    global_write_set.dstSet = global_descriptorset.set;
    global_write_set.descriptorCount = 1;
    global_write_set.descriptorType = vk::DescriptorType::eUniformBuffer;
    //Camera data write
    vk::WriteDescriptorSet& camera_write_set = write_sets[1];
    vk::DescriptorBufferInfo camera_buffer_info;
    camera_buffer_info.buffer = cam_gpu_data.get_buffer();
    camera_buffer_info.offset = 0;
    camera_buffer_info.range = sizeof(camera_data);
    camera_write_set.pBufferInfo = &camera_buffer_info;
    camera_write_set.dstBinding = 1;
    camera_write_set.dstSet = global_descriptorset.set;
    camera_write_set.descriptorCount = 1;
    camera_write_set.descriptorType = vk::DescriptorType::eUniformBuffer;
    vulkan::device.updateDescriptorSets(2, write_sets, 0, nullptr);
}

void render::scene_renderer::resize(std::uint32_t width, std::uint32_t height) {
    this->width = width;
    this->height = height;
    vulkan::device.destroyFramebuffer(framebuffer);
    color.resize(width, height);
    depth.resize(width, height);
    create_framebuffer();
}

void render::scene_renderer::record_render(vk::CommandBuffer& cmd) {
    vk::RenderPassBeginInfo renderpass_begin;
    renderpass_begin.renderPass = forward_renderpass;
    renderpass_begin.framebuffer = framebuffer;
    renderpass_begin.renderArea.offset.x = 0;
    renderpass_begin.renderArea.offset.y = 0;
    renderpass_begin.renderArea.extent.width = width;
    renderpass_begin.renderArea.extent.height = height;
    vk::ClearValue clear_values[2];
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 0.0f};
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;
    renderpass_begin.pClearValues = clear_values;
    renderpass_begin.clearValueCount = 2;
    cmd.beginRenderPass(renderpass_begin, vk::SubpassContents::eInline);
    bool first_time = true;
    if (scene_data != nullptr && camera != nullptr) {
        for (auto& [pipeline_id, materials] : scene_data->get_render_graph()) {
            const pipeline* forward_pipeline = get_forward_pipeline(pipeline_id);
            if (forward_pipeline == nullptr) continue;
            cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, forward_pipeline->pipeline);
            if (first_time) {
                first_time = false;
                vk::Viewport viewport;
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(width);
                viewport.height = static_cast<float>(height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vk::Rect2D scissor;
                scissor.offset.x = 0;
                scissor.offset.y = 0;
                scissor.extent.width = width;
                scissor.extent.height = height;
                cmd.setViewport(0, 1, &viewport);
                cmd.setScissor(0, 1, &scissor);
                cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, forward_pipeline->layout, 0, 1, &global_descriptorset.set, 0, nullptr);
            }
            for (const auto& [material_id, meshes] : materials) {
                const gpu_material* gpu_mat = get_or_create_gpu_material(material_id);
                if (gpu_mat == nullptr) continue; //TODO: check if is ready
                gpu_mat->record_bind_descriptorsets(cmd);
                gpu_mat->record_push_constants(cmd);
                for (const auto& [mesh_id, instance_data] : meshes) {
                    const gpu_mesh* mesh = gpu_mesh_manager::get_resource(mesh_id).first;
                    vk::DeviceSize offsets[1] = {0};
                    if (mesh == nullptr) continue;
                    mesh->record_bind_buffer(cmd);
                    vk::Buffer model_matrix_buffer = instance_data.get_model_matrix_buffer();
                    cmd.bindVertexBuffers(1, 1, &model_matrix_buffer, offsets);
                    cmd.drawIndexed(mesh->get_index_count(), instance_data.get_instance_count(), 0, 0, 0);
                }
            }
        }
    }
    cmd.endRenderPass();
}

void render::scene_renderer::record_sync_camera(vk::CommandBuffer& cmd) {
    if (camera == nullptr) return;
    glm::mat4 proj = camera->get_projection_matrix(width, height);
    if (proj == cam_data.projection && !cam_watcher.has_updated()) return;
    cam_data.projection = proj;
    cam_data.view = camera->get_view_matrix();
    cam_data.view_projection = camera->get_view_projection_matrix(width, height);
    cam_gpu_data.set(&cam_data);
    cam_gpu_data.record_upload(cmd);
}

void render::scene_renderer::select_scene(const uuid& id, const scenegraph::camera_node_ref& camera) {
    if (scene_data != nullptr) vulkan::free_descriptorset(global_descriptorset);
    scene_data = get_scene_render_data(id);;
    if (scene_data != nullptr) create_descriptorset();
    this->camera = camera;
    select_camera(this->camera);
}

void render::scene_renderer::select_scene(const uuid& id) {
    scenegraph::scene* s = scenegraph::get_scene(id);
    if (s == nullptr) {
        if (scene_data != nullptr) vulkan::free_descriptorset(global_descriptorset);
        scene_data = nullptr;
        return;
    }
    select_scene(id, s->get_default_camera());
}

void render::scene_renderer::select_camera(const scenegraph::camera_node_ref& camera) {
    cam_watcher = scenegraph::transform_watcher(camera->get_transform());
    this->camera = camera;
}