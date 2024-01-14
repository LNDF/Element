#include "vulkan_shader.h"

using namespace element;

vk::ShaderModule vulkan::create_shader_module(const std::vector<std::uint32_t>& spv)  {
    vk::ShaderModuleCreateInfo info;
    info.flags = vk::ShaderModuleCreateFlags();
    info.codeSize = spv.size() * sizeof(std::uint32_t);
    info.pCode = spv.data();
    return device.createShaderModule(info);
}