#pragma once

#include <istream>
#include <istream>
#include <string>
#include <utils/uuid.h>

namespace element {
    extern std::istream fs_get_resource(const uuid& id);
    extern std::string fs_get_resource_name(const uuid& id);
    extern uuid fs_get_uuid_from_resource_name(const std::string& name, const std::string& ns);
} // namespace element
