#pragma once

#include <cereal/types/polymorphic.hpp>

#define ELM_SERIALIZE_NVP(n, v) cereal::make_nvp(n, v)
#define ELM_SERIALIZE_BASE(c) cereal::base_class(c)
#define ELM_SERIALIZE_SIZE(s)   cereal::make_size_tag(static_cast<element::size64_t>(s))
#define ELM_DESERIALIZE_SIZE(s) cereal::make_size_tag(static_cast<std::size_t>(s))
#define ELM_REGISTER_SERIALIZABLE_DERIVED_TYPE(c) CEREAL_REGISTER_TYPE(c)
#define ELM_REGISTER_SERIALIZABLE_DERIVED_NAMED_TYPE(c, n) CEREAL_REGISTER_TYPE_WITH_NAME(c, n)
#define ELM_REGISTER_DERIVED_BASE_SERIALIZATION_RELATION(d, b) CEREAL_REGISTER_POLYMORPHIC_RELATION(b, d)

namespace element {

    typedef cereal::size_type size64_t;

} // namespace element
