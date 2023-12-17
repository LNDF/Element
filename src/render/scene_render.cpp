#include "scene_render.h"

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
    color_attachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;
    //Subpass's first attachment reference
    vk::AttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = vk::ImageLayout::eColorAttachmentOptimal;
    //Subpass
    vk::SubpassDescription subpass;
    subpass.flags = vk::SubpassDescriptionFlags();
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.pColorAttachments = &color_attachment_reference;
    subpass.colorAttachmentCount = 1;
    //Renderpass
    vk::RenderPassCreateInfo renderpass_info;
    renderpass_info.flags = vk::RenderPassCreateFlags();
    renderpass_info.attachmentCount = 1;
    renderpass_info.pAttachments = &color_attachment;
    renderpass_info.subpassCount = 1;
    renderpass_info.pSubpasses = &subpass;
    forward_renderpass = vulkan::device.createRenderPass(renderpass_info);
}

void render::scene_renderer::init() {
    create_forward_renderpass();
}

void render::scene_renderer::cleanup() {
    vulkan::device.destroyRenderPass(forward_renderpass);
}