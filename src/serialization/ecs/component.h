#pragma once

#include <cereal/types/polymorphic.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

#define ELM_REGISTER_SERIALIZABLE_COMPONENT(comp) \
    CEREAL_REGISTER_TYPE_WITH_NAME(element::component_pool<comp>, #comp) \
    CEREAL_REGISTER_POLYMORPHIC_RELATION(element::component_pool_base, element::component_pool<comp>)
