#ifndef GEAROENIX_CORE_MACROS_ZEROER_HPP
#define GEAROENIX_CORE_MACROS_ZEROER_HPP

#include <cstring>

#define GX_SET_ARRAY_ZERO(x)              \
    {                                     \
        std::memset((x), 0, sizeof((x))); \
    }

#define GX_SET_ZERO(x)                     \
    {                                      \
        std::memset(&(x), 0, sizeof((x))); \
    }
#endif