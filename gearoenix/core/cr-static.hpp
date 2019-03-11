#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#ifndef countof
#define countof(x) (sizeof(x) / sizeof(x[0]))
#endif
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
#endif
