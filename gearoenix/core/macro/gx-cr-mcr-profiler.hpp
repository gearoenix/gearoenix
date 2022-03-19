#ifndef GEAROENIX_CORE_MACROS_PROFILER_HPP
#define GEAROENIX_CORE_MACROS_PROFILER_HPP
#include "../../platform/gx-plt-log.hpp"
#include <chrono>

#define GX_PROFILE_BEGIN(tag) \
    const auto _gearoenix_core_macro_profile_begin_##tag = std::chrono::high_resolution_clock::now()

#define GX_PROFILE_END(tag)                                                                                                                                                   \
    const std::chrono::duration<double> _gearoenix_core_macro_profile_duration_##tag = std::chrono::high_resolution_clock::now() - _gearoenix_core_macro_profile_begin_##tag; \
    GX_LOG_I("Gearoenix Profile " #tag ": " << _gearoenix_core_macro_profile_duration_##tag.count() << "s")

#define GX_PROFILE_BEGIN_S(tag) \
    _gearoenix_core_macro_profile_begin_##tag = std::chrono::high_resolution_clock::now()

#define GX_PROFILE_END_S(tag)                                                                                                  \
    thread_local static std::chrono::time_point<std::chrono::high_resolution_clock> _gearoenix_core_macro_profile_begin_##tag; \
    GX_PROFILE_END(tag)

#endif