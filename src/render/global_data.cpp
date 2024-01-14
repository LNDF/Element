#include "global_data.h"

#include <render/vulkan_descriptor.h>
#include <render/vulkan_buffer.h>
#include <chrono>

using namespace element;

static render::global_data::data global_data_instance;
static vk::DescriptorSetLayout global_descriptorset_layout;
static vulkan::upload_buffer* global_buffer = nullptr;

static std::uint64_t epoch_tim_to_millis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void render::global_data::init() {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.reserve(2);
    vk::DescriptorSetLayoutBinding global_data_binding;
    global_data_binding.binding = 0;
    global_data_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
    global_data_binding.descriptorCount = 1;
    global_data_binding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics;
    global_data_binding.pImmutableSamplers = nullptr;
    bindings.push_back(global_data_binding);
    vk::DescriptorSetLayoutBinding camera_data_binding;
    camera_data_binding.binding = 1;
    camera_data_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
    camera_data_binding.descriptorCount = 1;
    camera_data_binding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics;
    camera_data_binding.pImmutableSamplers = nullptr;
    bindings.push_back(camera_data_binding);
    global_descriptorset_layout = vulkan::create_descriptorset_layout_from_bindings(bindings);
    global_buffer = new vulkan::upload_buffer(sizeof(global_data::data), vk::BufferUsageFlagBits::eUniformBuffer);
}

void render::global_data::cleanup() {
    delete global_buffer;
    vulkan::device.destroyDescriptorSetLayout(global_descriptorset_layout);
}

vk::DescriptorSetLayout render::global_data::get_descriptorset_layout() {
    return global_descriptorset_layout;
}

vk::Buffer render::global_data::get_data_buffer() {
    return global_buffer->get_buffer();
}

void render::global_data::record_sync(vk::CommandBuffer& cmd) {
    global_data_instance.time = epoch_tim_to_millis();
    global_buffer->set(&global_data_instance);
    global_buffer->record_upload(cmd);
}