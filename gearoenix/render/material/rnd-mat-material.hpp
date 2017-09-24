#ifndef GEAROEMIX_CORE_STATIC_HPP
#define GEAROEMIX_CORE_STATIC_HPP
#ifndef countof
#define countof(x) (sizeof(x) / sizeof(x[0]))
#ifdef setz
#error "setz already defined"
#endif
#include <cstring>
#define setz(x) std::memset(&x, 0, sizeof(x))
#endif
#endif
