#include <plugins/module.h>
#include <element/defs.h>
#include <element/modules/module.h>

using namespace element;

ELM_BEGIN_C

static const void* get_module(const char* name) {
    return plugins::get_module(name);
}

static int module_exists(const char* name) {
    return plugins::get_module(name) != nullptr;
}

static void register_module(const element_module_t* module) {
    plugins::register_module(module->name, module->data);
}

static void unregister_module(const char* name) {
    plugins::unregister_module(name);
}

static element_module_module_t module_module = {
    get_module,
    module_exists,
    register_module,
    unregister_module
};

ELM_END_C

ELM_REGISTER_MODULE("element.module", &module_module)