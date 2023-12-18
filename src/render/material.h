#pragma once

#include <render/pipeline.h>
#include <graphics/vulkan_buffer.h>
#include <graphics/vulkan_descriptor..h>
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

        class gpu_material;

        class material {
            private:
                friend class gpu_material;

                static constexpr std::uint32_t push_constants_index = std::numeric_limits<std::uint32_t>::max();

                std::vector<material_buffer> uniform_buffers;
                material_buffer push_constants_buffer;
                std::unordered_map<std::string, material_property> properties;
                pipeline_data* data = nullptr;
                uuid pipeline_id;
                
                std::pair<material_buffer*, const shader_block_member*> get_buffer_and_layout(const std::string& name);
                std::pair<const material_buffer*, const shader_block_member*> get_buffer_and_layout(const std::string& name) const;
            public:
                void init(bool reset_buffers);

                inline const uuid& get_pipeline_id() const {return pipeline_id;}
                inline const std::vector<material_buffer>& get_uniform_buffers() const {return uniform_buffers;}
                inline const material_buffer& get_push_constants_buffer() const {return push_constants_buffer;}
                inline const std::unordered_map<std::string, material_property>& get_properties() const {return properties;}
                inline pipeline_data* get_pipeline_data() {return data;}
                inline void set_pipeline_id(const uuid& id) {if (data == nullptr) pipeline_id = id;}
                
                template<typename T>
                void set_property(const std::string& name, const T& t);

                template<typename T>
                void get_property(const std::string& name, T& t) const;
                
                template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
                void set_property(const std::string& name, const glm::mat<C, R, T, Q>& mat);

                template<typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
                void get_property(const std::string& name, glm::mat<C, R, T, Q>& mat) const;

                std::uint32_t get_array_size(const std::string& name) const;
                std::uint32_t get_array2d_size(const std::string& name) const;

                template<typename T>
                void set_property_array(const std::string& name, const T* t);
                
                template<typename T>
                void get_property_array(const std::string& name, T* t) const;

                inline void __set_uniform_buffer(std::vector<material_buffer>&& buffer) {if (data == nullptr) uniform_buffers = std::move(buffer);}
                inline void __set_push_constants_buffer(material_buffer&& buffer) {if (data == nullptr) push_constants_buffer = std::move(buffer);}
        };

        class gpu_material {
            private:
                const pipeline* forward_pipeline = nullptr;
                material* cpu_material = nullptr;
                std::vector<vulkan::upload_buffer> uniform_buffers;
                std::vector<vulkan::descriptor_set> descriptorsets;
                mutable std::uint32_t references = 0;
            public:
                gpu_material(const uuid& id);
                ~gpu_material();

                gpu_material(const gpu_material& other) = delete;
                gpu_material(gpu_material&& other);
                gpu_material& operator=(const gpu_material& other) = delete;
                gpu_material& operator=(gpu_material&& other);

                bool record_sync_if_meeded(vk::CommandBuffer& cmd);
                void record_bind_descriptorsets(vk::CommandBuffer& cmd);
                void record_push_constants(vk::CommandBuffer& cmd);

                void claim() const;
                void release() const;

                inline bool is_valid() const {return cpu_material != nullptr;}
                inline bool is_being_used() const {return references > 0;}
                inline std::uint32_t get_references() const {return references;}
                inline void __set_references(std::uint32_t references) {this->references = references;}
        };
        
        gpu_material* get_gpu_material(const uuid& id);
        gpu_material* get_or_create_gpu_material(const uuid& id);
        void destroy_material(const uuid& id);
        void destroy_all_materials();
        void record_sync_materials(vk::CommandBuffer& cmd);

    } // namespace render
} // namespace element
