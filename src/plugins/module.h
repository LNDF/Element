#pragma once

#include <utils/pre_exec.h>
#include <unordered_map>
#include <string>

#define ELM_REGISTER_MODULE(name, module) ELM_PRE_EXECUTE(element::plugins::register_module, name, module)

namespace element {
    
    namespace plugins {
        
        void register_module(const std::string& name, void* module);
        void* get_module(const std::string& name);
        void unregister_module(const std::string& name);
        const std::unordered_map<std::string, void*>& get_modules();

    } // namespace plugins
} // namespace element

ELM_REGISTER_MODULE("test", nullptr)