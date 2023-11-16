#pragma once

#include <render/pipeline.h>
#include <utils/uuid.h>
#include <glm/glm.hpp>
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
            std::vector<std::uint8_t> data;
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
                
                template<typename T>
                void set_property(const std::string& name, const T& t);

                template<typename T>
                T get_property(const std::string& name);

                template<typename T>
                void set_array_property(const std::string& name, const std::vector<T>& vec);

                template<typename T>
                std::vector<T> get_array_property(const std::string& name);

                template<typename T>
                void set_array2d_property(const std::string& name, const std::vector<std::vector<T>>& vec);

                template<typename T>
                std::vector<std::vector<T>> get_array2d_property(const std::string& name);

                template<glm::length_t L, typename T, glm::qualifier Q>
                void set_vec_property(const std::string& name, const glm::vec<L, T, Q>& vec);

                template<glm::length_t L, typename T, glm::qualifier Q>
                glm::vec<L, T, Q> get_vec_property(const std::string& name);

                template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
                void set_mat_property(const std::string& name, const glm::mat<C, R, T, Q>& vec);

                template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
                glm::mat<C, R, T, Q> get_mat_property(const std::string& name);

        };

    } // namespace render
} // namespace element
