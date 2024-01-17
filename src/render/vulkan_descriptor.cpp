#include "vulkan_descriptor.h"

#include <event/event.h>
#include <render/render_events.h>
#include <render/render.h>
#include <vector>

static std::vector<vk::DescriptorPool> descriptor_pools;

static vk::DescriptorPoolSize pool_sizes[] = {
    {vk::DescriptorType::eUniformBuffer, 512}
};

using namespace element;

static std::vector<vulkan::descriptor_set> queued_deletions;

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
vulkan::descriptor_set vulkan::allocate_descriptorset(const vk::DescriptorSetLayout& layout) {
    vk::DescriptorSetAllocateInfo alloc_info;
    descriptor_set ret;
    alloc_info.pNext = nullptr;
    alloc_info.pSetLayouts = &layout;
    alloc_info.descriptorSetCount = 1;
    for (const vk::DescriptorPool& pool : descriptor_pools) {
        try {
            alloc_info.descriptorPool = pool;
            ret.set = device.allocateDescriptorSets(alloc_info)[0];
            ret.pool = pool;
            return ret;
        } catch (const vk::FragmentedPoolError&) {

        } catch (const vk::OutOfPoolMemoryError&) {

        }
    }
    vk::DescriptorPoolCreateInfo info;
    info.flags = vk::DescriptorPoolCreateFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    info.maxSets = 512;
    info.pPoolSizes = pool_sizes;
    info.poolSizeCount = 1;
    vk::DescriptorPool new_pool = device.createDescriptorPool(info);
    alloc_info.descriptorPool = new_pool;
    ret.set = device.allocateDescriptorSets(alloc_info)[0];
    ret.pool = new_pool;
    descriptor_pools.push_back(new_pool);
    return ret;
}

void vulkan::free_descriptorset(descriptor_set& set) {
    queued_deletions.push_back(set);
    render::reset_renderer_later();
}

void vulkan::clean_descriptorsets() {
    for (const vk::DescriptorPool& pool : descriptor_pools) {
        device.destroyDescriptorPool(pool);
    }
    descriptor_pools.clear();
}

static bool descriptorset_delete(events::render_idle& event) {
    for (auto& set : queued_deletions) {
        vulkan::device.freeDescriptorSets(set.pool, set.set);
    }
    queued_deletions.clear();
    return true;
}

ELM_REGISTER_EVENT_CALLBACK(events::render_idle, descriptorset_delete, event_callback_priority::highest)