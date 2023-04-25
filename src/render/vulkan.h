#pragma once

#include <cinttypes>
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <vector>
#include <functional>

#define ELM_REGISTER_VULKAN_REQUIRED_EXTENSION_HOOK(hook) static bool __elm_register_required_extension_hook_##hook = element::vulkan::preregister_required_extension_hook(hook)

namespace element {

    class vulkan {
        private:
            static std::unordered_set<std::string> supported_instance_extensions;
            static std::unordered_set<std::string> supported_instance_layers;
            static std::uint32_t version;

            static vk::Instance instance;
            static vk::DispatchLoaderDynamic dld;
#ifdef ELM_ENABLE_LOGGING
            static vk::DebugUtilsMessengerEXT debug_messenger;
#endif

        public:
            static void init();
            static void cleanup();

            static std::vector<std::function<void(std::vector<const char*>&)>>& get_preregistered_required_extension_hook();
            static bool preregister_required_extension_hook(std::function<void(std::vector<const char*>&)> hook);

            static inline std::uint32_t get_version() {return version;}
            static inline vk::Instance get_instance() {return instance;}
            static inline const std::unordered_set<std::string>& get_supported_instance_extensions() {return supported_instance_extensions;}
            static inline const std::unordered_set<std::string>& get_supported_instance_layers() {return supported_instance_layers;}
    };

} // namespace element
