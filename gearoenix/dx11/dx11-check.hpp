#ifndef GEAROENIX_DIRECTX11_CHECK_HPP
#define GEAROENIX_DIRECTX11_CHECK_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#define GXHRCHK(x)     \
    if (FAILED((x))) { \
        UNEXPECTED;    \
    }
#endif
#endif
