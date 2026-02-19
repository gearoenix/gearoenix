#pragma once
#include "gx-cr-build-configuration.hpp"
#if GX_PROFILING_MODE

namespace gearoenix::core::profiler {
void new_frame();
void begin(const char* tag);
void end(const char* tag);
void report();
}

#define GX_PROFILE_NEW_FRAME ::gearoenix::core::profiler::new_frame()
#define GX_PROFILE_BEGIN(tag) ::gearoenix::core::profiler::begin(#tag)
#define GX_PROFILE_END(tag) ::gearoenix::core::profiler::end(#tag)
#define GX_PROFILE_REPORT ::gearoenix::core::profiler::report()

#else
#define GX_PROFILE_NEW_FRAME static_assert(true, "")
#define GX_PROFILE_BEGIN(tag) static_assert(true, "")
#define GX_PROFILE_END(tag) static_assert(true, "")
#define GX_PROFILE_REPORT static_assert(true, "")
#endif