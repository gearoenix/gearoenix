#ifndef GEAROENIX_CORE_MACROS_STATIC_HPP
#define GEAROENIX_CORE_MACROS_STATIC_HPP
#include "gx-cr-build-configuration.hpp"
#include "gx-cr-types.hpp"
#include <cstring>
#include <memory>

#define GX_COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define GX_IS_ZERO(x) (((x) < (GX_POS_EPSILON)) && ((x) > (GX_NEG_EPSILON)))
#define GX_IS_NOT_ZERO(x) (((x) > (GX_POS_EPSILON)) || ((x) < (GX_NEG_EPSILON)))
#define GX_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define GX_MIN(x, y) (((x) < (y)) ? (x) : (y))

#define GX_CHECK_NOT_EQUAL(x, y)               \
    {                                          \
        if ((x) == (y)) {                      \
            GXLOGF("Inequality check failed.") \
        }                                      \
    }
#define GX_CHECK_EQAUL(x, y)                 \
    {                                        \
        if ((x) != (y)) {                    \
            GXLOGF("Equality check failed.") \
        }                                    \
    }
#ifdef GX_DEBUG_MODE
#define GX_CHECK_NOT_EQUAL_D(x, y) GX_CHECK_NOT_EQUAL(x, y)
#define GX_CHECK_EQUAL_D(x, y) GX_CHECK_EQAUL(x, y)
#else
#define GX_CHECK_NOT_EQUAL_D(x, y) \
    {                              \
        ((void)(x));               \
        ((void)(y));               \
    }
#define GX_CHECK_EQUAL_D(x, y) \
    {                          \
        ((void)(x));           \
        ((void)(y));           \
    }
#endif
#define GX_CONCAT_0(x, y) x##y
#define GX_CONCAT_1(x, y) GX_CONCAT_0(x, y)
#define GX_CONCAT_2(x, y) GX_CONCAT_1(x, y)

#ifdef GX_THREAD_NOT_SUPPORTED
#define GX_CREATE_GUARD(m)
#define GX_CREATE_GUARD_S(m)
#define GX_GUARD_LOCK(m)
#else
#include <atomic>
#include <mutex>
#define GX_CREATE_GUARD(m) std::mutex m##_lock;
#define GX_CREATE_GUARD_S(m) static GX_CREATE_GUARD(m)
#define GX_GUARD_LOCK(m) std::lock_guard<decltype(m##_lock)> GX_CONCAT_2(_gx_guard_lock_, __LINE__)(m##_lock);
#endif

#define GX_CEIL_2(x) (((x) >> 1) + ((x)&1))
#define GX_CEIL_4(x) (((x) >> 2) + ((((x) >> 1) & 1) | ((x)&1)))
#define GX_CEIL_8(x) (((x) >> 3) + ((((x) >> 2) & 1) | (((x) >> 1) & 1) | ((x)&1)))

#define GX_START_MULTITHREADED_TASKS \
    unsigned int task_number = 0;

#define GX_DO_MULTITHREADED_TASK(expr)                   \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }

#define GX_FLAG_CHECK(x, flag) ((static_cast<std::uint64_t>(x) & static_cast<std::uint64_t>(flag)) == static_cast<std::uint64_t>(flag))

#endif
