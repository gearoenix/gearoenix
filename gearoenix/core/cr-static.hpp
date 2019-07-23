#ifndef GEAROENIX_CORE_STATIC_HPP
#define GEAROENIX_CORE_STATIC_HPP
#define GXCOUNTOF(x) (sizeof(x) / sizeof(x[0]))
#include "cr-types.hpp"
#define GX_IS_ZERO(x) (((x) < (GXPOSEPSILON)) && ((x) > (GXNEGEPSILON)))
#define GXISNOTZERO(x) (((x) > (GXPOSEPSILON)) || ((x) < (GXNEGEPSILON)))
#ifndef GX_MAX
#define GX_MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef GX_MIN
#define GX_MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifdef setz
#error "setz already defined"
#endif
#include <cstring>
#define GXSETZ(x) std::memset(&(x), 0, sizeof((x)))
#define GXSETARRZ(x) std::memset((x), 0, sizeof((x)))
#define GX_GETTER_BUILDER(x) \
    decltype(x) get_##x() const { return x; }
#define GXDELETE(x)       \
    if ((x) != nullptr) { \
        delete (x);       \
        (x) = nullptr;    \
    }
#define GX_CHECK_NOT_EQAUL(x, y) if ((x) == (y)) GXLOGF("Un-equality check failed.")
#define GX_CHECK_EQAUL(x, y) if ((x) != (y)) GXLOGF("Equality check failed.")
#ifdef GX_DEBUG_MODE
#define GX_CHECK_NOT_EQAUL_D(x, y) GX_CHECK_NOT_EQAUL(x, y)
#define GX_CHECK_EQAUL_D(x, y) GX_CHECK_EQAUL(x, y)
#else
#define GX_CHECK_NOT_EQAUL_D(x, y) {((void)(x)); ((void)(y));}
#define GX_CHECK_EQAUL_D(x, y) {((void)(x)); ((void)(y));}
#endif
#endif
