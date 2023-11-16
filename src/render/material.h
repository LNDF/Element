#pragma once

#include <render/pipeline.h>
#include <utils/uuid.h>
#include <glm/glm.hpp>
#include <cinttypes>
#include <vector>
#include <limits>
#include <utility>
#include <optional>
#include <unordered_map>

namespace element {
    namespace render {
        struct material_property {
            std::uint32_t buffer_index;
            shader_block_member* property;
        };
        
        struct material_buffer {
            std::vector<std::uint8_t> data;
            bool needs_sync;
        };

        class material {
            private:
                static constexpr std::uint32_t push_constants_index = std::numeric_limits<std::uint32_t>::max();

                std::vector<material_buffer> uniform_buffers;
                material_buffer push_constants_buffer;
                std::unordered_map<std::string, material_property> properties;
                pipeline_data* data = nullptr;
                uuid pipeline_id;

                std::pair<material_buffer*, shader_block_member*> get_buffer_and_layout(const std::string& name);
            public:
                void init(bool reset_buffers);

                const material_buffer& get_material_buffer(std::uint32_t index);

                inline const uuid& get_pipeline_id() {return pipeline_id;}
                inline pipeline_data* get_pipeline_data() {return data;}
                inline const material_buffer& get_push_constants_buffer() {return get_material_buffer(push_constants_index);}
                
                template<typename T>
                void set_property(const std::string& name, const T& t);

                template<typename T>
                T get_property(const std::string& name);

                std::uint32_t get_array_size(const std::string& name);
                std::uint32_t get_array2d_size(const std::string& name);

                template<typename T>
                void set_property_array(const std::string& name, const T* t);
                
                template<typename T>
                void get_property_array(const std::string& name, T* t);
        };

    } // namespace render
} // namespace element
