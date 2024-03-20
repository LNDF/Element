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
                ELM_WARN("Couldn't find resource {0}", id.str());
                return std::nullopt;
            }
            T t;
            try {
                binary_deserializer deserialize = create_binary_deserializer(*stream);
                deserialize(t);
            } catch (const std::exception& e) {
                ELM_WARN("Couldn't deserialize resource {0}", id.str());
                return std::nullopt;
            }
            return t;
        }

    } // namespace resource
} // namespace element


#define ELM_MAKE_RESOURCE_LOADER(fun, type) inline std::optional<type> fun(const element::uuid& id) {return element::resource::load_resource<type>(id);}