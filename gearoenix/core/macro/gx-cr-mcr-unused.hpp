#ifndef GEAROENIX_CORE_MACROS_UNUSED_HPP
#define GEAROENIX_CORE_MACROS_UNUSED_HPP

#define GX_FORCE_KEEP_UNUSED(x)                     \
    {                                               \
        const void* volatile _dummy##__LINE__ = &x; \
        (void)_dummy##__LINE__;                     \
        (void)x;                                    \
    }                                               \
    static_assert(true, "")

#endif