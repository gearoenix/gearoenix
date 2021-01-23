#ifndef GEAROENIX_PLATFORM_WINDOWS_MAIN_ENTRY_HPP
#define GEAROENIX_PLATFORM_WINDOWS_MAIN_ENTRY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_WINDOWS
#include <Windows.h>

#define GX_MAIN_ENTRY_ARGS_DEF                        \
    [[maybe_unused]] HINSTANCE instance,              \
        [[maybe_unused]] HINSTANCE previous_instance, \
        [[maybe_unused]] LPSTR command_line,          \
        [[maybe_unused]] int show_command

#define GX_MAIN_ENTRY_ARGS instance, previous_instance, command_line, show_command

#define GX_MAIN_ENTRY(expr)                    \
    int WINAPI WinMain(GX_MAIN_ENTRY_ARGS_DEF) \
    {                                          \
        expr;                                  \
        return 0;                              \
    }

#endif
#endif