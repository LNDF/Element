#pragma once

#define __ELM_PRE_EXECUTE_CREATE_NAMEH(name, ctr) name ## ctr
#define __ELM_PRE_EXECUTE_CREATE_NAME(name, ctr) __ELM_PRE_EXECUTE_CREATE_NAMEH(name, ctr)
#define ELM_PRE_EXECUTE(fun, ...) static bool __ELM_PRE_EXECUTE_CREATE_NAME(__elm_pre_execute_, __COUNTER__) = ([]() -> bool {fun(__VA_ARGS__); return false;})();