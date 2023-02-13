#pragma once

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#include <iostream>

#define ELM_SERIALIZE_NVP(n, v) cereal::make_nvp(n, v)

namespace element {

    typedef cereal::BinaryInputArchive binary_deserializer;
    typedef cereal::BinaryOutputArchive binary_serializer;
    typedef cereal::JSONInputArchive text_deserializer;
    typedef cereal::JSONOutputArchive text_serializer;
    
    inline binary_deserializer create_binary_deserializer(std::istream& is) {
        return cereal::BinaryInputArchive(is);
    }

    inline binary_serializer create_binary_serializer(std::ostream& os) {
        return cereal::BinaryOutputArchive(os);
    }

    inline text_deserializer create_text_deserializer(std::istream& is) {
        return cereal::JSONInputArchive(is);
    }

    inline text_serializer create_text_serializer(std::ostream& os) {
        return cereal::JSONOutputArchive(os);
    }

} // namespace element
