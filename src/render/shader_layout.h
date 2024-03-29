#pragma once

#include <string>
#include <vector>
#include <cinttypes>

namespace element {
    namespace render {
        
        enum shader_block_member_type {
            unknown,
            boolean_type,
            sint8_type,
            sint16_type,
            sint32_type,
            sint64_type,
            uint8_type,
            uint16_type,
            uint32_type,
            uint64_type,
            float16_type,
            float32_type,
            float64_type,
            struct_type,
            sampler_type
        };

        struct shader_block_member {
            std::string name;
            shader_block_member_type type = shader_block_member_type::unknown;
            std::size_t size = 0;
            std::uint32_t offset = 0;
            std::size_t array_stride = 0;
            std::size_t matrix_stride = 0;
            std::uint32_t vecsize = 0;
            std::uint32_t columns = 0;
            std::uint32_t array_rows = 0;
            std::uint32_t array_cols = 0;
        };

        struct shader_resource_layout : public shader_block_member {
            std::uint32_t set = 0;
            std::uint32_t binding = 0;
            bool sampler_resource = false;
            std::vector<shader_block_member> members;
        };

        struct shader_layout {
            shader_resource_layout push_constants;
            std::vector<shader_resource_layout> descriptor_sets;
        };

    } // namespace render
} // namespace element
