#include "pipeline.h"

#include <core/log.h>
#include <core/fs.h>
#include <shader/loader.h>

using namespace element;

vk::ShaderModule render::load_shader(const uuid& id) {
    auto data = shader::load_shader(id);
    if (data == std::nullopt) return nullptr;
    return vulkan::create_shader_module(data->spv);
}