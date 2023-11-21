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
            const shader_block_member* property = nullptr;
        };
        
        struct material_buffer {
            std::vector<std::uint8_t> data;
            bool needs_sync = true;
            std::uint32_t set = 0, binding = 0;
        };

        class material {
            private:
                static constexpr std::uint32_t push_constants_index = std::numeric_limits<std::uint32_t>::max();

                std::vector<material_buffer> uniform_buffers;
                material_buffer push_constants_buffer;
                std::unordered_map<std::string, material_property> properties;
                pipeline_data* data = nullptr;
                uuid pipeline_id;

                std::pair<material_buffer*, const shader_block_member*> get_buffer_and_layout(const std::string& name);
            public:
                void init(bool reset_buffers);

                inline const uuid& get_pipeline_id() const {return pipeline_id;}
                inline const std::vector<material_buffer>& get_uniform_buffers() const {return uniform_buffers;}
                inline const material_buffer& get_push_constants_buffer() const {return push_constants_buffer;}
                inline const std::unordered_map<std::string, material_property>& get_properties() const {return properties;}
                inline pipeline_data* get_pipeline_data() {return data;}
                inline void set_pipeline_id(const uuid& id) {if (data != nullptr) pipeline_id = id;}
                
                template<typename T>
                void set_property(const std::string& name, const T& t);

                template<typename T>
                void get_property(const std::string& name, T& t);
                
                template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
                void set_property(const std::string& name, const glm::mat<C, R, T, Q>& mat);

                template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
                void get_property(const std::string& name, glm::mat<C, R, T, Q>& mat);

                std::uint32_t get_array_size(const std::string& name);
                std::uint32_t get_array2d_size(const std::string& name);

                template<typename T>
                void set_property_array(const std::string& name, const T* t);
                
                template<typename T>
                void get_property_array(const std::string& name, T* t);

                inline void __set_uniform_buffer(std::vector<material_buffer>&& buffer) {if (data != nullptr) uniform_buffers = std::move(buffer);}
                inline void __set_push_constants_buffer(material_buffer&& buffer) {if (data != nullptr) push_constants_buffer = std::move(buffer);}
        };

    } // namespace render
} // namespace element
