#ifndef GEAROENIX_PLATFORM_SDL2_MAIN_ENTRY_HPP
#define GEAROENIX_PLATFORM_SDL2_MAIN_ENTRY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#ifdef GX_PLATFORM_LINUX
#include <SDL2/SDL_main.h>
#else
#include <SDL_main.h>
#endif

#define GX_MAIN_ENTRY_ARGS_DEF \
    [[maybe_unused]] int argc, \
        [[maybe_unused]] char **argv

#define GX_MAIN_ENTRY_ARGS argc, argv

#define GX_MAIN_ENTRY(expr)                                  \
    extern SDLMAIN_DECLSPEC int main(GX_MAIN_ENTRY_ARGS_DEF) \
    {                                                        \
        expr;                                                \
        return 0;                                            \
    }

#endif
#endif