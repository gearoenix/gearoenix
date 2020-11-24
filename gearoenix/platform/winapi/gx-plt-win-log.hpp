#ifndef GEAROENIX_PLATFORM_WINAPI_LOG_HPP
#define GEAROENIX_PLATFORM_WINAPI_LOG_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLT_WINAPI

#include <Windows.h>

#ifdef GX_LOG_F
#undef GX_LOG_F
#endif

#define GX_LOG_F(s)                                                                                       \
    {                                                                                                     \
        GX_PLT_LOG_COMMON(s, FATAL)                                                                       \
        (void)MessageBoxA(nullptr, GX_PLT_LOG_SS_VAR.str().c_str(), "Fatal error", MB_OK | MB_ICONERROR); \
        std::terminate();                                                                                 \
    }

#endif
#endif