#ifndef GEAROENIX_PLATFORM_MACRO_LOCK_HPP
#define GEAROENIX_PLATFORM_MACRO_LOCK_HPP
#include "../gx-plt-build-configuration.hpp"

#ifdef GX_PLT_THREAD_NOT_SUPPORTED
#define GX_CREATE_GUARD(m)
#define GX_CREATE_GUARD_S(m)
#define GX_GUARD_LOCK(m)
#else
#include "../../core/macro/gx-cr-mcr-concatenate.hpp"
#include <atomic>
#include <mutex>
#define GX_CREATE_GUARD(m) std::mutex m##_lock;
#define GX_CREATE_GUARD_S(m) static GX_CREATE_GUARD(m)
#define GX_GUARD_LOCK(m) std::lock_guard<decltype(m##_lock)> GX_CONCAT_2(_gearoenix_platform_guard_lock_, __LINE__)(m##_lock);
#endif

#endif