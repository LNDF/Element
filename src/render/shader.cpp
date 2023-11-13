#include "shader.h"

#include <core/fs.h>
#include <core/log.h>
#include <memory>

using namespace element;

std::vector<std::uint32_t> render::load_shader_spv(const uuid& id) {
    std::unique_ptr<std::istream> stream = fs::get_resource(id);
    std::vector<std::uint32_t> spv;
    if (stream->fail()) {
        ELM_WARN("Couldn't load shader {0}", id.str());
        return spv;
    }
    stream->seekg(0, std::ios::end);
    std::size_t stream_size = stream->tellg();
    stream->seekg(0, std::ios::beg);
    spv.resize(stream_size / sizeof(std::uint32_t));
    stream->read((char*) &spv[0], stream_size);
    return spv;
}