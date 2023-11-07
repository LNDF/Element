#pragma once

#include <string>
#include <vector>
#include <cinttypes>

namespace element {
    namespace shader {
        
        enum member_type {
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

        struct member_layout {
            std::string name;
            member_type type = member_type::unknown;
            std::size_t size = 0;
            std::uint32_t offset = 0;
            std::size_t array_stride = 0;
            std::size_t matrix_stride = 0;
            std::uint32_t vecsize = 0;
            std::uint32_t columns = 0;
            std::uint32_t array_rows = 0;
            std::uint32_t array_cols = 0;
        };

        struct resource_layout : public member_layout {
            std::uint32_t set = 0;
            std::uint32_t binding = 0;
            bool sampler_resource = false;
            std::vector<member_layout> members;
        };

        struct layout {
            resource_layout push_constants;
            std::vector<resource_layout> descriptor_sets;
        };

    } // namespace render
} // namespace element
