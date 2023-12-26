#pragma once

#include <ostream>
#include <string>
#include <cstdint>

namespace element {
    
    class uuid {
        private:
            static uuid null_uuid;
        public:
            union {
                std::uint8_t bytes[16];
                struct {
                    std::uint64_t low;
                    std::uint64_t high;
                };
                struct {
                    std::uint32_t time_low;
                    std::uint16_t time_mid;
                    std::uint16_t time_hi_and_version;
                    std::uint8_t  clk_seq_hi_res;
                    std::uint8_t  clk_seq_low;
                    std::uint8_t  node[6];
                };
            };
            uuid();
            uuid(const uuid& uuid);
            explicit uuid(const std::string& uuid); //CEREAL: (https://github.com/USCiLab/cereal/issues/415)
            

            std::string str() const;
            bool operator==(const uuid& other) const;
            void regenerate();
            void set_from_str(const std::string& uuid);
            bool is_null() const;

            static inline const uuid& null() {return null_uuid;}

            static void reseed_generator();
    };

} // namespace element

template<>
struct std::hash<element::uuid> {
    using result_type = std::size_t;
    using argument_type = element::uuid;

    std::size_t operator()(const element::uuid& k) const;
};

std::ostream& operator<<(std::ostream& os, const element::uuid& data);