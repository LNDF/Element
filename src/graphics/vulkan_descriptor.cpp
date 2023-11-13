#include "vulkan_descriptor..h"

using namespace element;

vk::DescriptorSetLayout vulkan::create_empty_descriptorset_layout() {
    vk::DescriptorSetLayoutCreateInfo info;
    info.flags = vk::DescriptorSetLayoutCreateFlags();
    info.bindingCount = 0;
    info.pBindings = nullptr;
    return device.createDescriptorSetLayout(info);
}

vk::DescriptorSetLayout vulkan::create_descriptorset_layout_from_bindings(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
    vk::DescriptorSetLayoutCreateInfo info;
    info.flags = vk::DescriptorSetLayoutCreateFlags();
    info.bindingCount = bindings.size();
    info.pBindings = bindings.data();
    return device.createDescriptorSetLayout(info);
}