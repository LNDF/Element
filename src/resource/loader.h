#pragma once

#include <core/fs.h>
#include <core/log.h>
#include <utils/uuid.h>
#include <serialization/defs.h>
#include <serialization/serializers.h>
#include <optional>

namespace element {
    namespace resource {
        
        template<typename T>
        std::optional<T> load_resource(const uuid& id) {
            std::unique_ptr<std::istream> stream = fs::get_resource(id);
            if (stream->fail()) {
                ELM_WARN("Couldn't load resource {0}", id.str());
                return std::nullopt;
            }
            T t;
            binary_deserializer deserialize = create_binary_deserializer(*stream);
            deserialize(t);
            return t;
        }

    } // namespace resource
} // namespace element


#define ELM_MAKE_RESOURCE_LOADER(fun, type) inline std::optional<type> fun(const element::uuid& id) {return element::resource::load_resource<type>(id);}