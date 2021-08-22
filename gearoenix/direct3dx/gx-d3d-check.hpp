#ifndef GEAROENIX_DIRECT3DX_CHECK_HPP
#define GEAROENIX_DIRECT3DX_CHECK_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../platform/gx-plt-log.hpp"
#include <winerror.h>

#define GX_D3D_CHECK(x)  \
    {                    \
        if (FAILED(x)) { \
            GX_LOG_F(#x) \
        }                \
    }

#endif
#endif