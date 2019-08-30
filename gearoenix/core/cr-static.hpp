#ifndef GEAROENIX_CORE_STATIC_HPP
#define GEAROENIX_CORE_STATIC_HPP

#include "cr-types.hpp"
#include <cstring>

#define GX_GET_REF_PRV(t, x) private: t x; public: const t & get_##x() const noexcept { return x; } t & get_##x() noexcept { return x; }
#define GX_GET_PTR_PRV(t, x) private: t * x = nullptr; public: const t * get_##x() const noexcept { return x; } t * get_##x() noexcept { return x; }
#define GX_GET_ARR(v, t, x, c) v: t x[c] = {}; public: const t (&get_##x() const noexcept)[c] { return x; } t (&get_##x() noexcept)[c] { return x; }
#define GX_GET_ARR_PRV(t, x, c) GX_GET_ARR(private, t, x, c)
#define GX_GET_VAL(v, t, x, d) v: t x = d; public: t get_##x() const noexcept { return x; }
#define GX_GET_VAL_PRT(t, x, d) GX_GET_VAL(protected, t, x, d)
#define GX_GET_VAL_PRV(t, x, d) GX_GET_VAL(private, t, x, d)
#define GX_COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define GX_IS_ZERO(x) (((x) < (GXPOSEPSILON)) && ((x) > (GXNEGEPSILON)))
#define GX_IS_NOT_ZERO(x) (((x) > (GXPOSEPSILON)) || ((x) < (GXNEGEPSILON)))
#define GX_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define GX_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define GX_SET_ZERO(x) { std::memset(&(x), 0, sizeof((x))); }
#define GX_SET_ARRAY_ZERO(x) { std::memset((x), 0, sizeof((x))); }
#define GX_DELETE(x) if ((x) != nullptr) { delete (x); (x) = nullptr; }
#define GX_CHECK_NOT_EQAUL(x, y) { if ((x) == (y)) { GXLOGF("Un-equality check failed.") } }
#define GX_CHECK_EQAUL(x, y) { if ((x) != (y)) { GXLOGF("Equality check failed.") } }
#ifdef GX_DEBUG_MODE
#define GX_CHECK_NOT_EQAUL_D(x, y) GX_CHECK_NOT_EQAUL(x, y)
#define GX_CHECK_EQAUL_D(x, y) GX_CHECK_EQAUL(x, y)
#else
#define GX_CHECK_NOT_EQAUL_D(x, y) {((void)(x)); ((void)(y));}
#define GX_CHECK_EQAUL_D(x, y) {((void)(x)); ((void)(y));}
#endif
#define GX_CEIL_2(x) (((x) >> 1) + ((x) & 1))
#define GX_CEIL_4(x) (((x) >> 2) + ((((x) >> 1) & 1) | ((x) & 1)))
#define GX_CEIL_8(x) (((x) >> 3) + ((((x) >> 2) & 1) | (((x) >> 1) & 1) | ((x) & 1)))
#endif
