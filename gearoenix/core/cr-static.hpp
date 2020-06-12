#ifndef GEAROENIX_CORE_STATIC_HPP
#define GEAROENIX_CORE_STATIC_HPP
#include "cr-build-configuration.hpp"
#include "cr-types.hpp"
#include <cstring>
#include <memory>

#define GX_GET_CREF(v, c, t, x) \
    v:                          \
    c t x;                      \
                                \
public:                         \
    [[nodiscard]] const t& get_##x() const noexcept { return x; }
#define GX_GET_REF(v, t, x) \
    GX_GET_CREF(v, , t, x)  \
    [[nodiscard]] t& get_##x() noexcept { return x; }
#define GX_GETSET_CREF(v, t, x)         \
    GX_GET_CREF(v, , t, x)              \
    void set_##x(const t& arg) noexcept \
    {                                   \
        x = arg;                        \
    }
#define GX_GET_CREF_PRT(t, x) GX_GET_CREF(protected, , t, x)
#define GX_GET_CREF_PRV(t, x) GX_GET_CREF(private, , t, x)
#define GX_GET_REFC_PRV(t, x) GX_GET_CREF(private, const, t, x)
#define GX_GET_REF_PRT(t, x) GX_GET_REF(protected, t, x)
#define GX_GET_REF_PRV(t, x) GX_GET_REF(private, t, x)
#define GX_GETSET_CREF_PRT(t, x) GX_GETSET_CREF(protected, t, x)
#define GX_GETSET_CREF_PRV(t, x) GX_GETSET_CREF(private, t, x)

#define GX_GET_UPTR(v, c, t, x)                                         \
    v:                                                                  \
    c std::unique_ptr<t> x;                                             \
                                                                        \
public:                                                                 \
    [[nodiscard]] const t* get_##x() const noexcept { return x.get(); } \
    [[nodiscard]] t* get_##x() noexcept { return x.get(); }
#define GX_GET_UPTR_PRT(t, x) GX_GET_UPTR(protected, , t, x)
#define GX_GET_UPTR_PRV(t, x) GX_GET_UPTR(private, , t, x)
#define GX_GETSET_UPTR(v, t, x) \
    GX_GET_UPTR(v, , t, x)      \
    void set_##x(std::unique_ptr<t> _##x) noexcept { x = std::move(_##x); }

#define GX_GET_UCPTR(v, t, x) GX_GET_UPTR(v, const, t, x)
#define GX_GET_UCPTR_PRT(t, x) GX_GET_UCPTR(protected, t, x)
#define GX_GET_UCPTR_PRV(t, x) GX_GET_UCPTR(private, t, x)
#define GX_GETSET_UPTR_PRT(t, x) GX_GETSET_UPTR(protected, t, x)

#define GX_GET_PTRC(v, cc, c, t, x) \
    v:                              \
    cc t* c x = nullptr;            \
                                    \
public:                             \
    [[nodiscard]] const t* get_##x() const noexcept { return x; }
#define GX_GET_PTRC_PRV(t, x) GX_GET_PTRC(private, , const, t, x)
#define GX_GET_PTR(v, c, t, x) \
    GX_GET_PTRC(v, , c, t, x)  \
    [[nodiscard]] t* get_##x() noexcept { return x; }
#define GX_GET_PTR_PRT(t, x) GX_GET_PTR(protected, , t, x)
#define GX_GET_PTR_PRV(t, x) GX_GET_PTR(private, , t, x)
#define GX_GET_CPTR_PRT(t, x) GX_GET_PTR(protected, const, t, x)
#define GX_GET_CPTR_PRV(t, x) GX_GET_PTR(private, const, t, x)
#define GX_GETSET_PTR(v, t, x) \
    GX_GET_PTR(v, , t, x)      \
    void set_##x(t* const _##x) noexcept { x = _##x; }
#define GX_GETSET_PTRC(v, t, x)   \
    GX_GET_PTRC(v, const, , t, x) \
    void set_##x(const t* const _##x) noexcept { x = _##x; }
#define GX_GETSET_PTR_PRT(t, x) GX_GETSET_PTR(protected, t, x)
#define GX_GETSET_PTR_PRV(t, x) GX_GETSET_PTR(private, t, x)
#define GX_GETSET_PTRC_PRV(t, x) GX_GETSET_PTRC(private, t, x)

#define GX_GET_CARR(v, t, x, c) \
    v:                          \
    const t x[c];               \
                                \
public:                         \
    [[nodiscard]] const t(&get_##x() const noexcept)[c] { return x; }

#define GX_GET_CARR_PRV(t, x, c) GX_GET_CARR(private, t, x, c)

#define GX_GET_ARRC(v, t, x, c) \
    v:                          \
    t x[c] = {};                \
                                \
public:                         \
    [[nodiscard]] const t(&get_##x() const noexcept)[c] { return x; }

#define GX_GET_ARR(v, t, x, c) \
    GX_GET_ARRC(v, t, x, c)    \
    [[nodiscard]] t(&get_##x() noexcept)[c] { return x; }

#define GX_GET_ARRC_PRV(t, x, c) GX_GET_ARRC(private, t, x, c)
#define GX_GET_ARR_PRV(t, x, c) GX_GET_ARR(private, t, x, c)

#define GX_GET_CVAL(v, t, x) \
    v:                       \
    const t x;               \
                             \
public:                      \
    [[nodiscard]] t get_##x() const noexcept { return x; }
#define GX_GET_CVAL_PRT(t, x) GX_GET_CVAL(protected, t, x)
#define GX_GET_CVAL_PRV(t, x) GX_GET_CVAL(private, t, x)

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
#define GX_GET_AVAL_PRT(t, x, d) GX_GET_AVAL(protected, t, x, d)

#define GX_GETSET_AVAL(v, t, x, d) \
    GX_GET_AVAL(v, t, x, d)        \
    void set_##x(const t _##x) noexcept { x = _##x; }
#define GX_GETSET_AVAL_PRT(t, x, d) GX_GETSET_AVAL(protected, t, x, d)
#define GX_GETSET_AVAL_PRV(t, x, d) GX_GETSET_AVAL(private, t, x, d)

#define GX_COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define GX_IS_ZERO(x) (((x) < (GX_POS_EPSILON)) && ((x) > (GX_NEG_EPSILON)))
#define GX_IS_NOT_ZERO(x) (((x) > (GX_POS_EPSILON)) || ((x) < (GX_NEG_EPSILON)))
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
#define GX_CHECK_NOT_EQUAL(x, y)               \
    {                                          \
        if ((x) == (y)) {                      \
            GXLOGF("Inequality check failed.") \
        }                                      \
    }
#define GX_CHECK_EQAUL(x, y)                 \
    {                                        \
        if ((x) != (y)) {                    \
            GXLOGF("Equality check failed.") \
        }                                    \
    }
#ifdef GX_DEBUG_MODE
#define GX_CHECK_NOT_EQUAL_D(x, y) GX_CHECK_NOT_EQUAL(x, y)
#define GX_CHECK_EQUAL_D(x, y) GX_CHECK_EQAUL(x, y)
#else
#define GX_CHECK_NOT_EQUAL_D(x, y) \
    {                              \
        ((void)(x));               \
        ((void)(y));               \
    }
#define GX_CHECK_EQUAL_D(x, y) \
    {                          \
        ((void)(x));           \
        ((void)(y));           \
    }
#endif
#define GX_CONCAT_0(x, y) x##y
#define GX_CONCAT_1(x, y) GX_CONCAT_0(x, y)
#define GX_CONCAT_2(x, y) GX_CONCAT_1(x, y)

#ifdef GX_THREAD_NOT_SUPPORTED
#define GX_CREATE_GUARD(m)
#define GX_CREATE_GUARD_S(m)
#define GX_GUARD_LOCK(m)
#else
#include <atomic>
#include <mutex>
#define GX_CREATE_GUARD(m) std::mutex m##_lock;
#define GX_CREATE_GUARD_S(m) static GX_CREATE_GUARD(m)
#define GX_GUARD_LOCK(m) std::lock_guard<decltype(m##_lock)> GX_CONCAT_2(_gx_guard_lock_, __LINE__)(m##_lock);
#endif

#define GX_CEIL_2(x) (((x) >> 1) + ((x)&1))
#define GX_CEIL_4(x) (((x) >> 2) + ((((x) >> 1) & 1) | ((x)&1)))
#define GX_CEIL_8(x) (((x) >> 3) + ((((x) >> 2) & 1) | (((x) >> 1) & 1) | ((x)&1)))

#define GX_START_MULTITHREADED_TASKS \
    unsigned int task_number = 0;

#define GX_DO_MULTITHREADED_TASK(expr)                   \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }

#endif
