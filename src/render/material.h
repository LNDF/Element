#pragma once

#include <render/pipeline.h>
#include <utils/uuid.h>
#include <cinttypes>
#include <vector>
#include <limits>
#include <unordered_map>

namespace element {
    namespace render {
        
        static constexpr std::uint32_t push_constants_index = std::numeric_limits<std::uint32_t>::max();

        struct material_property {
            std::uint32_t buffer_index;
            shader_block_member* property;
        };
        
        struct material_buffer {
            std::vector<std::uint32_t> data;
            bool needs_sync;
        };

        class material {
            private:
                std::vector<material_buffer> uniform_buffers;
                material_buffer push_constants_buffer;
                std::unordered_map<std::string, material_property> properties;
                pipeline_data* data = nullptr;
                uuid pipeline_id;
            public:
                void init(bool reset_buffers);

                const material_buffer& get_material_buffer(std::uint32_t index);

                inline const uuid& get_pipeline_id() {return pipeline_id;}
                inline pipeline_data* get_pipeline_data() {return data;}
                inline const material_buffer& get_push_constants_buffer() {return get_material_buffer(push_constants_index);}
                
        };

    } // namespace render
} // namespace element
