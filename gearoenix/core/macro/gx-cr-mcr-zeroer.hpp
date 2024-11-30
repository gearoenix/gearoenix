#pragma once
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