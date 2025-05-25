#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include <SDL2/SDL_main.h>

#define GX_MAIN_ENTRY_ARGS_DEF \
    [[maybe_unused]] int argc, \
        [[maybe_unused]] char **argv

#define GX_MAIN_ENTRY_ARGS argc, argv

#if GX_PLATFORM_WEBASSEMBLY || GX_PLATFORM_LINUX
#define GX_SDL_MAIN int main
#else
#define GX_SDL_MAIN extern "C" SDLMAIN_DECLSPEC int main
#endif

#define GX_MAIN_ENTRY(expr)             \
    GX_SDL_MAIN(GX_MAIN_ENTRY_ARGS_DEF) \
    {                                   \
        expr;                           \
        return 0;                       \
    }

#endif