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
    create_forward_renderpass();
}

void render::scene_renderer::cleanup() {
    vulkan::device.destroyRenderPass(forward_renderpass);
}