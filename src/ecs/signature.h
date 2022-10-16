#pragma once

#include <bitset>
#include <cstdint>

#define MAX_COMPONENT_COUNT 32

namespace engine {
    
    class signature_manager {
        public:
            using signature = std::bitset<MAX_COMPONENT_COUNT>;
            static inline std::uint32_t get_gomponent_count() {return counter;}

            template<typename T>
            static std::uint32_t get_bit_position() {
                static std::uint32_t pos = counter++;
                return pos;
            }

            template<typename... T>
            static signature get_signature() {
                signature signature;
                (signature.set(get_bit_position<T>(), true), ...);
                return signature;
            }
        private:
            static std::uint32_t counter;
    };

} // namespace engine