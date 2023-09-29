#pragma once

#include <utility>

namespace element {
    
    template<typename T, typename...Args>
    bool call_and_return_true(T func, Args&&... args) {
        func(std::forward<Args>(args)...);
        return true;
    }

} // namespace element

#define __ELM_PRE_EXECUTE_CREATE_NAMEH(name, ctr) name ## ctr
#define __ELM_PRE_EXECUTE_CREATE_NAME(name, ctr) __ELM_PRE_EXECUTE_CREATE_NAMEH(name, ctr)
#define ELM_PRE_EXECUTE(...) static bool __ELM_PRE_EXECUTE_CREATE_NAME(__elm_pre_execute_, __COUNTER__) = element::call_and_return_true(__VA_ARGS__);