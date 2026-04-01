#pragma once
#if GX_PROFILING_ENABLED
#include <tracy/Tracy.hpp>

#define GX_PROFILE_NEW_FRAME FrameMark
#define GX_PROFILE_SCOPE(tag) ZoneScopedN(#tag)

#else

#define GX_PROFILE_NEW_FRAME static_assert(true, "")
#define GX_PROFILE_SCOPE(tag) static_assert(true, "")

#endif