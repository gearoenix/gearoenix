#pragma once

#define GX_FORCE_KEEP_UNUSED(x)                     \
    {                                               \
        const void* volatile _dummy##__LINE__ = &x; \
        (void)_dummy##__LINE__;                     \
        (void)x;                                    \
    }                                               \
    static_assert(true, "")