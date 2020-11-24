#ifndef GEAROENIX_PLATFORM_WINAPI_MAIN_ENTRY_HPP
#define GEAROENIX_PLATFORM_WINAPI_MAIN_ENTRY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLT_WINAPI
#include <WinBase.h>

#define GX_MAIN_ENTRY_ARGS                            \
    [[maybe_unused]] HINSTANCE instance,              \
        [[maybe_unused]] HINSTANCE previous_instance, \
        [[maybe_unused]] LPSTR command_line,          \
        [[maybe_unused]] int show_command

#define GX_MAIN_ENTRY(expr)                \
    int WINAPI WinMain(GX_MAIN_ENTRY_ARGS) \
    {                                      \
        expr;                              \
    }

#endif
#endif