#pragma once
#include "../../platform/gx-plt-log.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "gx-cr-mcr-stringifier.hpp"

#define GX_COMPLAIN(x, msg)                                         \
    {                                                               \
        if (!(x)) {                                                 \
            GX_LOG_E("Soft check failed: " GX_STRINGIFY(x) << msg); \
        }                                                           \
    }                                                               \
    static_assert(true, "")

#define GX_ASSERT(x)                                        \
    {                                                       \
        if (!(x)) {                                         \
            GX_LOG_F("Assertion failed: " GX_STRINGIFY(x)); \
        }                                                   \
    }                                                       \
    static_assert(true, "")

#define GX_CHECK_NOT_EQUAL(x, y)                                                                   \
    {                                                                                              \
        if ((x) == (y)) {                                                                          \
            GX_LOG_F("Not-equality check of " GX_STRINGIFY(x) " and " GX_STRINGIFY(y) " failed."); \
        }                                                                                          \
    }                                                                                              \
    static_assert(true, "")

#define GX_CHECK_EQUAL(x, y)                                                                   \
    {                                                                                          \
        if ((x) != (y)) {                                                                      \
            GX_LOG_F("Equality check of " GX_STRINGIFY(x) " and " GX_STRINGIFY(y) " failed."); \
        }                                                                                      \
    }                                                                                          \
    static_assert(true, "")

#ifdef GX_DEBUG_MODE
#define GX_ASSERT_D(x) GX_ASSERT(x)
#define GX_COMPLAIN_D(x, msg) GX_COMPLAIN(x, msg)
#define GX_CHECK_NOT_EQUAL_D(x, y) GX_CHECK_NOT_EQUAL(x, y)
#define GX_CHECK_EQUAL_D(x, y) GX_CHECK_EQUAL(x, y)
#else
#define GX_ASSERT_D(x) static_assert(true, "")
#define GX_COMPLAIN_D(x, msg) static_assert(true, "")
#define GX_CHECK_NOT_EQUAL_D(x, y) static_assert(true, "")
#define GX_CHECK_EQUAL_D(x, y) static_assert(true, "")
#endif