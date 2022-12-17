#pragma once

#include <ostream>
#include <string>
#include <cstdint>

namespace element {
    
    class uuid {
        public:
            union {
                struct {
                    std::uint32_t time_low;
                    std::uint16_t time_mid;
                    std::uint16_t time_hi_and_version;
                    std::uint8_t  clk_seq_hi_res;
                    std::uint8_t  clk_seq_low;
                    std::uint8_t  node[6];
                };
                std::uint8_t bytes[16];
            };
            uuid();
            uuid(const uuid& uuid);
            uuid(const std::string& uuid);
            

            std::string str() const;
            bool operator==(const uuid& other) const;
    };

} // namespace element

template<>
struct std::hash<element::uuid> {
    using result_type = std::size_t;
    using argument_type = element::uuid;

    std::size_t operator()(const element::uuid& k) const;
};

std::ostream& operator<<(std::ostream& os, const element::uuid& data);