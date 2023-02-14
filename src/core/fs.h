#pragma once

#include <istream>
#include <string>
#include <memory>
#include <utils/uuid.h>

namespace element {

    enum fs_resource_type {
        dummy
    };

    struct fs_resource_info {
        std::string path;
        fs_resource_type type;
    };

    extern std::unique_ptr<std::istream> fs_get_resource(const uuid& id);
    extern const fs_resource_info& fs_get_resource_info(const uuid& id);
    extern const uuid& fs_get_uuid_from_resource_path(const std::string& path);
    extern void fs_load_resources();
    
} // namespace element
