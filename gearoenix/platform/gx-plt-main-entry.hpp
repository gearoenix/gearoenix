#ifndef GEAROENIX_PLATFORM_MAIN_ENTRY_HPP
#define GEAROENIX_PLATFORM_MAIN_ENTRY_HPP
#include "gx-plt-build-configuration.hpp"

#ifdef GX_PLATFORM_WINDOWS
#include "windows/gx-plt-win-main-entry.hpp"
#elif defined(GX_PLATFORM_LINUX)

#define GX_MAIN_ENTRY_ARGS           \
    [[maybe_unused]] const int argc, \
        [[maybe_unused]] char const *const *const argv

#define GX_MAIN_ENTRY(expr)      \
    int main(GX_MAIN_ENTRY_ARGS) \
    {                            \
        expr;                    \
        return 0;                \
    }

#else
#error "Unimplemented or unexpected platform interface."
#endif

#endif