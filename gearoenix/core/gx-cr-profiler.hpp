#pragma once
#if GX_PROFILING_ENABLED
#include <tracy/Tracy.hpp>

#define GX_PROFILE_NEW_FRAME FrameMark
#define GX_PROFILE_SCOPE(tag) ZoneScopedN(#tag)
#define GX_PROFILE_EXP(exp) \
    do {                    \
        ZoneScopedN(#exp);  \
        exp;                \
    } while (false)

#else

#define GX_PROFILE_NEW_FRAME static_assert(true, "")
#define GX_PROFILE_SCOPE(tag) static_assert(true, "")
#define GX_PROFILE_EXP(exp) exp

#endif