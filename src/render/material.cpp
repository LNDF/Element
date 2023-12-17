#include "material.h"

#include <render/pipeline_manager.h>
#include <render/pipeline.h>
#include <render/material_manager.h>
#include <unordered_map>

using namespace element;

static std::unordered_map<uuid, render::gpu_material> loaded_gpu_materials;

template<typename T>
static void write_to_buffer(const T& t, render::material_buffer& buffer, std::uint32_t offset) {
    T* dest = (T*) &buffer.data[offset];
    *dest = t;
    buffer.needs_sync = true;
}

template<typename T>
static void read_from_buffer(T& t, const render::material_buffer& buffer, std::uint32_t offset) {
    const T* ptr = (const T*) &buffer.data[offset];
    t = *ptr;
}

template<typename T, glm::length_t L, glm::qualifier Q>
static void write_to_buffer(const glm::vec<L, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset) {
    for (glm::length_t i = 0; i < L; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += sizeof(T);
    }
}

template<typename T, glm::length_t L, glm::qualifier Q>
static void read_from_buffer(glm::vec<L, T, Q>& t, const render::material_buffer& buffer, std::uint32_t offset) {
    for (glm::length_t i = 0; i < L; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += sizeof(T);
    }
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
static void write_to_buffer(const glm::mat<C, R, T, Q>& t, render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (glm::length_t i = 0; i < C; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
static void read_from_buffer(glm::mat<C, R, T, Q>& t, const render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (glm::length_t i = 0; i < C; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T>
static void write_array_to_buffer(const T* t, std::uint32_t size, render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (std::uint32_t i = 0; i < size; ++i) {
        write_to_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

template<typename T>
static void read_array_from_buffer(T* t, std::uint32_t size, const render::material_buffer& buffer, std::uint32_t offset, std::uint32_t stride) {
    for (std::uint32_t i = 0; i < size; ++i) {
        read_from_buffer(t[i], buffer, offset);
        offset += stride;
    }
}

void render::material::init(bool reset_buffers) {
    data = pipeline_data_manager::get(pipeline_id);
    if (data == nullptr) return;
    for (const auto& member : data->push_constants.first.members) {
        material_property prop;
        prop.buffer_index = material::push_constants_index;
        prop.property = &member;
        properties[member.name] = std::move(prop);
    }
    if (reset_buffers) {
        uniform_buffers.clear();
        properties.clear();
        push_constants_buffer.binding = 0;
        push_constants_buffer.set = 0;
        push_constants_buffer.needs_sync = true;
        push_constants_buffer.data.resize(data->push_constants.first.size);
        for (const auto& [layout, stage] : data->layouts) {
            if (layout.set == 0) continue;
            material_buffer buffer;
            buffer.data.resize(layout.size);
            buffer.set = layout.set;
            buffer.binding = layout.binding;
            std::uint32_t index = uniform_buffers.size();
            uniform_buffers.push_back(std::move(buffer));
            for (const auto& member : layout.members) {
                material_property prop;
                prop.buffer_index = index;
                prop.property = &member;
                properties[member.name] = std::move(prop);
            }
        }
    } else {
        std::uint32_t index = 0;
        for (const auto& buffer : uniform_buffers) {
            for (const auto& [layout, stage] : data->layouts) {
                if (buffer.set == layout.set && buffer.binding == layout.binding) {
                    for (const auto& member : layout.members) {
                        material_property prop;
                        prop.buffer_index = index;
                        prop.property = &member;
                        properties[member.name] = std::move(prop);
                    }
                    break;
                }
            }
            ++index;
        }
    }
}

std::pair<render::material_buffer*, const render::shader_block_member*> render::material::get_buffer_and_layout(const std::string& name) {
    auto prop_it = properties.find(name);
    if (prop_it == properties.end()) return std::make_pair(nullptr, nullptr);
    std::pair<material_buffer*, const render::shader_block_member*> result;
    if (prop_it->second.buffer_index == material::push_constants_index) {
        result.first = &push_constants_buffer;
    } else {
        result.first = &uniform_buffers[prop_it->second.buffer_index];
    }
    result.second = prop_it->second.property;
    return result;
}

std::pair<const render::material_buffer*, const render::shader_block_member*> render::material::get_buffer_and_layout(const std::string& name) const {
    auto prop_it = properties.find(name);
    if (prop_it == properties.end()) return std::make_pair(nullptr, nullptr);
    std::pair<const material_buffer*, const render::shader_block_member*> result;
    if (prop_it->second.buffer_index == material::push_constants_index) {
        result.first = &push_constants_buffer;
    } else {
        result.first = &uniform_buffers[prop_it->second.buffer_index];
    }
    result.second = prop_it->second.property;
    return result;
}

template<typename T>
void render::material::set_property(const std::string& name, const T& t) {
    auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    write_to_buffer(t, *buffer, layout->offset);
}

template<typename T>
void render::material::get_property(const std::string& name, T& t) const {
    const auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    read_from_buffer(t, *buffer, layout->offset);
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
void render::material::set_property(const std::string& name, const glm::mat<C, R, T, Q>& mat) {
    auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    write_to_buffer(mat, *buffer, layout->offset, layout->matrix_stride);
}

template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
void render::material::get_property(const std::string& name, glm::mat<C, R, T, Q>& mat) const {
    const auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    read_from_buffer(mat, *buffer, layout->offset, layout->matrix_stride);
}

std::uint32_t render::material::get_array_size(const std::string& name) const {
    const auto [buffer, layout] = get_buffer_and_layout(name);
    if (layout == nullptr) return 0;
    return layout->array_cols;
}

std::uint32_t render::material::get_array2d_size(const std::string& name) const {
    const auto [buffer, layout] = get_buffer_and_layout(name);
    if (layout == nullptr) return 0;
    return layout->array_rows;
}

template<typename T>
void render::material::set_property_array(const std::string& name, const T* t) {
    auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    std::uint32_t array_size = layout->array_cols;
    std::uint32_t array2d_size = layout->array_rows;
    std::uint32_t stride = layout->array_stride;
    if (array2d_size > 0) {
        array_size *= array2d_size;
        stride /= array2d_size;
    }
    write_array_to_buffer(t, array_size, *buffer, layout->offset, stride);
}

template<typename T>
void render::material::get_property_array(const std::string& name, T* t) const {
    const auto [buffer, layout] = get_buffer_and_layout(name);
    if (buffer == nullptr || layout == nullptr) return;
    std::uint32_t array_size = layout->array_cols;
    std::uint32_t array2d_size = layout->array_rows;
    if (array2d_size > 0) array_size *= array2d_size;
    std::uint32_t stride = layout->array_stride;
    if (array2d_size > 0) {
        array_size *= array2d_size;
        stride /= array2d_size;
    }
    read_array_from_buffer(t, array_size, *buffer, layout->offset, stride);
}

render::gpu_material::gpu_material(const uuid& id) {
    cpu_material = material_manager::get(id);
    if (cpu_material == nullptr) return;
    forward_pipeline = get_forward_pipeline(cpu_material->pipeline_id);
    if (forward_pipeline == nullptr) return;
    descriptorsets.reserve(forward_pipeline->descriptorset_layouts.size());
    for (const auto& layout : forward_pipeline->descriptorset_layouts) {
        descriptorsets.push_back(vulkan::allocate_descriptorset(layout));
    }
    uniform_buffers.reserve(cpu_material->uniform_buffers.size());
    for (const auto& buffer : cpu_material->uniform_buffers) {
        auto& gpu_buffer = uniform_buffers.emplace_back(buffer.data.size(), vk::BufferUsageFlagBits::eUniformBuffer);
        vk::WriteDescriptorSet write_set;
        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = gpu_buffer.get_buffer();
        buffer_info.offset = 0;
        buffer_info.range = buffer.data.size();
        write_set.pBufferInfo = &buffer_info;
        write_set.dstBinding = buffer.binding;
        write_set.dstSet = descriptorsets[buffer.set - 1].set;
        write_set.descriptorCount = 1;
        write_set.descriptorType = vk::DescriptorType::eUniformBuffer;
        vulkan::device.updateDescriptorSets(1, &write_set, 0, nullptr);
    }
}

render::gpu_material::~gpu_material() {
    if (cpu_material == nullptr) return;
    for (auto& descriptorset : descriptorsets) {
        vulkan::free_descriptorset(descriptorset);
    }
}

render::gpu_material::gpu_material(gpu_material&& other)
    : forward_pipeline(std::move(other.forward_pipeline)), cpu_material(std::move(other.cpu_material)),
      uniform_buffers(std::move(other.uniform_buffers)), descriptorsets(std::move(descriptorsets)) {
    other.forward_pipeline = nullptr;
    other.cpu_material = nullptr;
}

render::gpu_material& render::gpu_material::operator=(gpu_material&& other) {
    for (auto& descriptorset : descriptorsets) {
        vulkan::free_descriptorset(descriptorset);
    }
    forward_pipeline = std::move(other.forward_pipeline);
    cpu_material = std::move(other.cpu_material);
    uniform_buffers = std::move(other.uniform_buffers);
    descriptorsets = std::move(other.descriptorsets);
    other.forward_pipeline = nullptr;
    other.cpu_material = nullptr;
    return *this;
}

bool render::gpu_material::record_sync_if_meeded(vk::CommandBuffer& cmd) {
    bool synced = false;
    std::uint32_t index = 0;
    for (auto& buffer : cpu_material->uniform_buffers) {
        if (buffer.needs_sync) {
            synced = true;
            buffer.needs_sync = false;
            uniform_buffers[index].set(buffer.data.data());
            uniform_buffers[index].record_upload(cmd);
        }
        ++index;
    }
    return synced;
}

void render::gpu_material::record_bind_descriptorsets(vk::CommandBuffer& cmd) {
    std::uint32_t index = 1;
    for (const auto& set : descriptorsets) {
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, forward_pipeline->layout, index++, 1, &set.set, 0, nullptr);
    }
}

void render::gpu_material::record_push_constants(vk::CommandBuffer& cmd) {
    cmd.pushConstants(forward_pipeline->layout, cpu_material->data->push_constants.second, 0, static_cast<std::uint32_t>(cpu_material->push_constants_buffer.data.size()), cpu_material->push_constants_buffer.data.data());
}

render::gpu_material* render::get_gpu_material(const uuid& id) {
    auto it = loaded_gpu_materials.find(id);
    if (it == loaded_gpu_materials.end()) {
        gpu_material new_mat(id);
        if (!new_mat.is_valid()) return nullptr;
        it = loaded_gpu_materials.insert_or_assign(id, std::move(new_mat)).first;
    }
    return &it->second;
}

void render::gpu_material::claim() const {
    ++references;
}

void render::gpu_material::release() const {
    --references;
}

void render::destroy_material(const uuid& id) {
    loaded_gpu_materials.erase(id);
    material_manager::destroy(id);
}

void render::destroy_all_materials() {
    loaded_gpu_materials.clear();
    material_manager::destroy_all();
}

#define MATERIAL_TYPE_ARRAY(...) template void render::material::set_property_array<__VA_ARGS__>(const std::string& name, const __VA_ARGS__* t); \
                                 template void render::material::get_property_array<__VA_ARGS__>(const std::string& name, __VA_ARGS__* t) const;

#define MATERIAL_TYPE(...) template void render::material::set_property<__VA_ARGS__>(const std::string& name, const __VA_ARGS__& t); \
                           template void render::material::get_property<__VA_ARGS__>(const std::string& name, __VA_ARGS__& t) const; \
                           MATERIAL_TYPE_ARRAY(__VA_ARGS__)

#define MATERIAL_TYPE_MAT(T, C, R, Q) template void render::material::set_property<T, C, R, Q>(const std::string& name, const glm::mat<C, R, T, Q>& mat); \
                                      template void render::material::get_property<T, C, R, Q>(const std::string& name, glm::mat<C, R, T, Q>& mat) const; \
                                      MATERIAL_TYPE_ARRAY(glm::mat<C, R, T, Q>)

#define MATERIAL_TYPE_VEC(T, L, Q) MATERIAL_TYPE(glm::vec<L, T, Q>)

#define MATERIAL_TYPE_NUMERIC(T) MATERIAL_TYPE(T) \
                                 MATERIAL_TYPE_MAT(T, 2, 2, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 2, 3, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 2, 4, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 3, 2, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 3, 3, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 3, 4, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 4, 2, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 4, 3, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 4, 4, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_MAT(T, 2, 2, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 2, 3, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 2, 4, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 3, 2, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 3, 3, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 3, 4, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 4, 2, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 4, 3, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 4, 4, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_MAT(T, 2, 2, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 2, 3, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 2, 4, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 3, 2, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 3, 3, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 3, 4, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 4, 2, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 4, 3, glm::qualifier::highp) \
                                 MATERIAL_TYPE_MAT(T, 4, 4, glm::qualifier::highp) \
                                 MATERIAL_TYPE_VEC(T, 1, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_VEC(T, 2, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_VEC(T, 3, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_VEC(T, 4, glm::qualifier::lowp) \
                                 MATERIAL_TYPE_VEC(T, 1, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_VEC(T, 2, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_VEC(T, 3, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_VEC(T, 4, glm::qualifier::mediump) \
                                 MATERIAL_TYPE_VEC(T, 1, glm::qualifier::highp) \
                                 MATERIAL_TYPE_VEC(T, 2, glm::qualifier::highp) \
                                 MATERIAL_TYPE_VEC(T, 3, glm::qualifier::highp) \
                                 MATERIAL_TYPE_VEC(T, 4, glm::qualifier::highp)

MATERIAL_TYPE(bool)
MATERIAL_TYPE_NUMERIC(std::int8_t)
MATERIAL_TYPE_NUMERIC(std::int16_t)
MATERIAL_TYPE_NUMERIC(std::int32_t)
MATERIAL_TYPE_NUMERIC(std::int64_t)
MATERIAL_TYPE_NUMERIC(std::uint8_t)
MATERIAL_TYPE_NUMERIC(std::uint16_t)
MATERIAL_TYPE_NUMERIC(std::uint32_t)
MATERIAL_TYPE_NUMERIC(std::uint64_t)
MATERIAL_TYPE_NUMERIC(float)
MATERIAL_TYPE_NUMERIC(double)