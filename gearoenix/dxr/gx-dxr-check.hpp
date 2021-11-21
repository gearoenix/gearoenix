#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../platform/gx-plt-log.hpp"
#include <winerror.h>

#define GX_DXR_CHECK(x)  \
    {                    \
        if (FAILED(x)) { \
            GX_LOG_F(#x) \
        }                \
    }

#endif