#pragma once

#include <istream>
#include <string>
#include <memory>
#include <unordered_map>
#include <utils/uuid.h>

namespace element {

    struct fs_resource_info {
        std::string path;
        std::string type;
    };

    namespace fs {
        extern std::unique_ptr<std::istream> get_resource(const uuid& id);
        extern const fs_resource_info& get_resource_info(const uuid& id);
        extern const uuid& get_uuid_from_resource_path(const std::string& path);
        extern void load_resources();
        extern const std::unordered_map<element::uuid, element::fs_resource_info>& get_map();
    } // namespace fs
    
} // namespace element
