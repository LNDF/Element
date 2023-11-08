#include "pipeline.h"

#include <core/log.h>
#include <core/fs.h>

using namespace element;

vk::ShaderModule render::load_shader(const uuid& id) {
    std::unique_ptr<std::istream> spv_stream = fs::get_resource(id);
    if (spv_stream->fail()) {
        ELM_WARN("Failed to load shader {0}", id.str());
        return nullptr;
    }
    spv_stream->seekg(0, std::ios::end);
    std::size_t stream_size = spv_stream->tellg();
    spv_stream->seekg(0, std::ios::beg);
    std::vector<std::uint32_t> spv;
    spv.resize(stream_size / sizeof(std::uint32_t));
    spv_stream->read((char*) &spv[0], stream_size);
    return vulkan::create_shader_module(spv);
}