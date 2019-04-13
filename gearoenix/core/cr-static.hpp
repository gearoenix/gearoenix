#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#define GXCOUNTOF(x) (sizeof(x) / sizeof(x[0]))
#include "cr-types.hpp"
#define GXISZERO(x) (((x) < (GXPOSEPSILON)) && ((x) > (GXNEGEPSILON)))
#define GXISNOTZERO(x) (((x) > (GXPOSEPSILON)) || ((x) < (GXNEGEPSILON)))
#ifndef GXMAX
#define GXMAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef GXMIN
#define GXMIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifdef setz
#error "setz already defined"
#endif
#include <cstring>
#define GXSETZ(x) std::memset(&(x), 0, sizeof((x)))
#define GXSETARRZ(x) std::memset((x), 0, sizeof((x)))
#define GX_GETTER_BUILDER(x) decltype(x) get_##x() const { return x; }
#endif
