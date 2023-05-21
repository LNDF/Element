#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define ELM_PLATFORM_WINDOWS
    #ifdef _WIN64
        #define ELM_PLATFORM_WIN64
    #else
        #define ELM_PLATFORM_WIN32
    #endif
#elif __APPLE__
    #error "Apple is not supported"
    #define ELM_PLATFORM_APPLE
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #define ELM_PLATFORM_IOS_SIMULATOR
    #elif TARGET_OS_MACCATALYST
        #define ELM_PLATFORM_MACCATALYST
    #elif TARGET_OS_IPHONE
        #define ELM_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define ELM_PLATFORM_MAC
    #else
        #error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    #error "Android is not supported"
    #define ELM_PLATFORM_ANDROID
#elif __linux__
    #define ELM_PLATFORM_LINUX
#elif __unix__
    #error "This UNIX platform is not supported"
    #define ELM_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
    #error "This POSIX platform is not supported"
    #define ELM_PLATFORM_POSIX
#else
    #error "Unknown platform"
#endif