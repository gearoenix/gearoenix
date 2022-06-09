#ifndef GEAROENIX_CORE_MACROS_ZEROER_HPP
#define GEAROENIX_CORE_MACROS_ZEROER_HPP
#include <cstring>

#define GX_SET_ARRAY_ZERO(x)              \
    {                                     \
        std::memset((x), 0, sizeof((x))); \
    }                                     \
    static_assert(true, "")

#define GX_SET_ZERO(x)                     \
    {                                      \
        std::memset(&(x), 0, sizeof((x))); \
    }                                      \
    static_assert(true, "")

#define GX_SET_VECTOR_ZERO(x)                                                 \
    {                                                                         \
        std::memset(x.data(), 0, sizeof(decltype(x)::value_type) * x.size()); \
    }                                                                         \
    static_assert(true, "")

#endif