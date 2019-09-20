#ifndef GEAROENIX_CORE_STATIC_HPP
#define GEAROENIX_CORE_STATIC_HPP

#include "cr-types.hpp"
#include <atomic>
#include <cstring>
#include <memory>

#define GX_GET_CREF(v, t, x) \
    v:                       \
    t x;                     \
                             \
public:                      \
    [[nodiscard]] const t& get_##x() const noexcept { return x; }
#define GX_GET_REF(v, t, x) \
    GX_GET_CREF(v, t, x)    \
    [[nodiscard]] t& get_##x() noexcept { return x; }
#define GX_GET_CREF_PRT(t, x) GX_GET_CREF(protected, t, x)
#define GX_GET_CREF_PRV(t, x) GX_GET_CREF(private, t, x)
#define GX_GET_REF_PRT(t, x) GX_GET_REF(protected, t, x)
#define GX_GET_REF_PRV(t, x) GX_GET_REF(private, t, x)

#define GX_GET_UPTR(v, c, t, x)                           \
    v:                                                    \
    c std::unique_ptr<t> x;                               \
                                                          \
public:                                                   \
    const t* get_##x() const noexcept { return x.get(); } \
    t* get_##x() noexcept { return x.get(); }
#define GX_GET_UPTR_PRT(t, x) GX_GET_UPTR(protected, , t, x)
#define GX_GET_UPTR_PRV(t, x) GX_GET_UPTR(private, , t, x)
#define GX_GETSET_UPTR(v, t, x) \
    GX_GET_UPTR(v, , t, x)      \
    void set_##x(std::unique_ptr<t> _##x) noexcept { x = std::move(_##x); }

#define GX_GET_UCPTR(v, t, x) GX_GET_UPTR(v, const, t, x)
#define GX_GET_UCPTR_PRT(t, x) GX_GET_UCPTR(protected, t, x)
#define GX_GET_UCPTR_PRV(t, x) GX_GET_UCPTR(private, t, x)
#define GX_GETSET_UPTR_PRT(t, x) GX_GETSET_UPTR(protected, t, x)

#define GX_GET_PTR(v, t, x)                         \
    v:                                              \
    t* x = nullptr;                                 \
                                                    \
public:                                             \
    const t* get_##x() const noexcept { return x; } \
    t* get_##x() noexcept { return x; }
#define GX_GET_PTR_PRV(t, x) GX_GET_PTR(private, t, x)
#define GX_GET_PTR_PRT(t, x) GX_GET_PTR(protected, t, x)
#define GX_GETSET_PTR(v, t, x) \
    GX_GET_PTR(v, t, x)        \
    void set_##x(t* const _##x) noexcept { x = _##x; }
#define GX_GETSET_PTR_PRT(t, x) GX_GETSET_PTR(protected, t, x)

#define GX_GET_PTRC(v, t, x)                                      \
    v:                                                            \
    t* const x;                                                   \
                                                                  \
public:                                                           \
    [[nodiscard]] const t* get_##x() const noexcept { return x; } \
    [[nodiscard]] t* get_##x() noexcept { return x; }
#define GX_GET_PTRC_PRT(t, x) GX_GET_PTRC(protected, t, x)

#define GX_GET_ARR(v, t, x, c)                          \
    v:                                                  \
    t x[c] = {};                                        \
                                                        \
public:                                                 \
    const t(&get_##x() const noexcept)[c] { return x; } \
    t(&get_##x() noexcept)[c] { return x; }
#define GX_GET_ARR_PRV(t, x, c) GX_GET_ARR(private, t, x, c)
#define GX_GET_CVAL(v, t, x) \
    v:                       \
    const t x;               \
                             \
public:                      \
    [[nodiscard]] t get_##x() const noexcept { return x; }
#define GX_GET_CVAL_PRT(t, x) GX_GET_CVAL(protected, t, x)

#define GX_GET_VAL(v, t, x, d) \
    v:                         \
    t x = d;                   \
                               \
public:                        \
    [[nodiscard]] t get_##x() const noexcept { return x; }
#define GX_GETSET_VAL(v, t, x, d) \
    GX_GET_VAL(v, t, x, d)        \
    void set_##x(const t _##x) noexcept { x = _##x; }
#define GX_GET_VAL_PRT(t, x, d) GX_GET_VAL(protected, t, x, d)
#define GX_GET_VAL_PRV(t, x, d) GX_GET_VAL(private, t, x, d)
#define GX_GETSET_VAL_PRT(t, x, d) GX_GETSET_VAL(protected, t, x, d)
#define GX_GETSET_VAL_PRV(t, x, d) GX_GETSET_VAL(protected, t, x, d)

#define GX_GET_AVAL(v, t, x, d) \
    v:                          \
    std::atomic<t> x = d;       \
                                \
public:                         \
    [[nodiscard]] t get_##x() const noexcept { return x; }
#define GX_GETSET_AVAL(v, t, x, d) \
    GX_GET_AVAL(v, t, x, d)        \
    void set_##x(const t _##x) noexcept { x = _##x; }
#define GX_GETSET_AVAL_PRT(t, x, d) GX_GETSET_AVAL(protected, t, x, d)
#define GX_GETSET_AVAL_PRV(t, x, d) GX_GETSET_AVAL(private, t, x, d)

#define GX_COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define GX_IS_ZERO(x) (((x) < (GXPOSEPSILON)) && ((x) > (GXNEGEPSILON)))
#define GX_IS_NOT_ZERO(x) (((x) > (GXPOSEPSILON)) || ((x) < (GXNEGEPSILON)))
#define GX_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define GX_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define GX_SET_ZERO(x)                     \
    {                                      \
        std::memset(&(x), 0, sizeof((x))); \
    }
#define GX_SET_ARRAY_ZERO(x)              \
    {                                     \
        std::memset((x), 0, sizeof((x))); \
    }
#define GX_DELETE(x)      \
    if ((x) != nullptr) { \
        delete (x);       \
        (x) = nullptr;    \
    }
#define GX_CHECK_NOT_EQAUL(x, y)               \
    {                                          \
        if ((x) == (y)) {                      \
            GXLOGF("Unequality check failed.") \
        }                                      \
    }
#define GX_CHECK_EQAUL(x, y)                 \
    {                                        \
        if ((x) != (y)) {                    \
            GXLOGF("Equality check failed.") \
        }                                    \
    }
#ifdef GX_DEBUG_MODE
#define GX_CHECK_NOT_EQAUL_D(x, y) GX_CHECK_NOT_EQAUL(x, y)
#define GX_CHECK_EQAUL_D(x, y) GX_CHECK_EQAUL(x, y)
#else
#define GX_CHECK_NOT_EQAUL_D(x, y) \
    {                              \
        ((void)(x));               \
        ((void)(y));               \
    }
#define GX_CHECK_EQAUL_D(x, y) \
    {                          \
        ((void)(x));           \
        ((void)(y));           \
    }
#endif
#define GX_CEIL_2(x) (((x) >> 1) + ((x)&1))
#define GX_CEIL_4(x) (((x) >> 2) + ((((x) >> 1) & 1) | ((x)&1)))
#define GX_CEIL_8(x) (((x) >> 3) + ((((x) >> 2) & 1) | (((x) >> 1) & 1) | ((x)&1)))
#endif
