#pragma once

#include <cereal/archives/binary.hpp>

#include <iostream>

namespace element {
    typedef cereal::BinaryInputArchive binary_deserializer;
    typedef cereal::BinaryOutputArchive binary_serializer;

    inline binary_deserializer create_binary_deserializer(std::istream& is) {
        return cereal::BinaryInputArchive(is);
    }

    inline binary_serializer create_binary_serializer(std::ostream& os) {
        return cereal::BinaryOutputArchive(os);
    }
} // namespace element
