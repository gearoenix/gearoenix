#ifndef GEAROENIX_CORE_MACRO_GETTER_SETTER_HPP
#define GEAROENIX_CORE_MACRO_GETTER_SETTER_HPP

#define GX_GET_CREF(v, c, t, x)            \
public:                                    \
    [[nodiscard]] const t& get_##x() const \
    {                                      \
        return x;                          \
    }                                      \
    v:                                     \
    c t x
#define GX_GET_REF(v, t, x)    \
public:                        \
    [[nodiscard]] t& get_##x() \
    {                          \
        return x;              \
    }                          \
    GX_GET_CREF(v, , t, x)
#define GX_GETSET_CREF(v, t, x) \
public:                         \
    void set_##x(const t& arg)  \
    {                           \
        x = arg;                \
    }                           \
    GX_GET_CREF(v, , t, x)
#define GX_GET_CREF_PRT(t, x) GX_GET_CREF(protected, , t, x)
#define GX_GET_CREF_PRV(t, x) GX_GET_CREF(private, , t, x)
#define GX_GET_REFC_PRV(t, x) GX_GET_CREF(private, const, t, x)
#define GX_GET_REFC_PRT(t, x) GX_GET_CREF(protected, const, t, x)
#define GX_GET_REF_PRT(t, x) GX_GET_REF(protected, t, x)
#define GX_GET_REF_PRV(t, x) GX_GET_REF(private, t, x)
#define GX_GETSET_CREF_PRT(t, x) GX_GETSET_CREF(protected, t, x)
#define GX_GETSET_CREF_PRV(t, x) GX_GETSET_CREF(private, t, x) // todo investigate the move too
#define GX_GET_UPTR(v, c, t, x)            \
public:                                    \
    [[nodiscard]] const t* get_##x() const \
    {                                      \
        return x.get();                    \
    }                                      \
    [[nodiscard]] t* get_##x()             \
    {                                      \
        return x.get();                    \
    }                                      \
    v:                                     \
    c std::unique_ptr<t> x
#define GX_GET_UPTR_PRT(t, x) GX_GET_UPTR(protected, , t, x)
#define GX_GET_UPTR_PRV(t, x) GX_GET_UPTR(private, , t, x)
#define GX_GETSET_UPTR(v, t, x)           \
public:                                   \
    void set_##x(std::unique_ptr<t> _##x) \
    {                                     \
        x = std::move(_##x);              \
    }                                     \
    GX_GET_UPTR(v, , t, x)

#define GX_GET_UCPTR(v, t, x) GX_GET_UPTR(v, const, t, x)
#define GX_GET_UCPTR_PRT(t, x) GX_GET_UCPTR(protected, t, x)
#define GX_GET_UCPTR_PRV(t, x) GX_GET_UCPTR(private, t, x)
#define GX_GETSET_UPTR_PRT(t, x) GX_GETSET_UPTR(protected, t, x)

#define GX_GET_PTR_C(v, cc, c, t, x)       \
public:                                    \
    [[nodiscard]] const t* get_##x() const \
    {                                      \
        return x;                          \
    }                                      \
    v:                                     \
    cc t* c x = nullptr
#define GX_GET_PTR(v, c, t, x) \
public:                        \
    [[nodiscard]] t* get_##x() \
    {                          \
        return x;              \
    }                          \
    GX_GET_PTR_C(v, , c, t, x)
#define GX_GET_PTRC_PRV(t, x) GX_GET_PTR(private, const, t, x)
#define GX_GET_PTRC_PRT(t, x) GX_GET_PTR(protected, const, t, x)
#define GX_GET_PTR_PRT(t, x) GX_GET_PTR(protected, , t, x)
#define GX_GET_PTR_PRV(t, x) GX_GET_PTR(private, , t, x)
#define GX_GET_CPTR_PRT(t, x) GX_GET_PTR_C(protected, const, , t, x)
#define GX_GET_CPTR_PRV(t, x) GX_GET_PTR_C(private, const, , t, x)
#define GX_GET_CPTRC_PRV(t, x) GX_GET_PTR_C(private, const, const, t, x)
#define GX_GET_CPTRC_PRT(t, x) GX_GET_PTR_C(protected, const, const, t, x)
#define GX_GETSET_PTR(v, t, x)  \
public:                         \
    void set_##x(t* const _##x) \
    {                           \
        x = _##x;               \
    }                           \
    GX_GET_PTR(v, , t, x)
#define GX_GETSET_PTRC(v, t, x)       \
public:                               \
    void set_##x(const t* const _##x) \
    {                                 \
        x = _##x;                     \
    }                                 \
    GX_GET_PTR_C(v, const, , t, x)
#define GX_GETSET_PTR_PRT(t, x) GX_GETSET_PTR(protected, t, x)
#define GX_GETSET_PTR_PRV(t, x) GX_GETSET_PTR(private, t, x)
#define GX_GETSET_PTRC_PRV(t, x) GX_GETSET_PTRC(private, t, x)

#define GX_GET_CARR(v, t, x, c)                           \
public:                                                   \
    [[nodiscard]] const std::array<t, c>& get_##x() const \
    {                                                     \
        return x;                                         \
    }                                                     \
    v:                                                    \
    const std::array<t, c> x
#define GX_GET_CARR_PRV(t, x, c) GX_GET_CARR(private, t, x, c)
#define GX_GET_ARRC(v, t, x, c)                           \
public:                                                   \
    [[nodiscard]] const std::array<t, c>& get_##x() const \
    {                                                     \
        return x;                                         \
    }                                                     \
    v:                                                    \
    std::array<t, c> x
#define GX_GET_ARR(v, t, x, c)                \
public:                                       \
    [[nodiscard]] std::array<t, c>& get_##x() \
    {                                         \
        return x;                             \
    }                                         \
    GX_GET_ARRC(v, t, x, c)
#define GX_GET_ARRC_PRV(t, x, c) GX_GET_ARRC(private, t, x, c)
#define GX_GET_ARR_PRV(t, x, c) GX_GET_ARR(private, t, x, c)
#define GX_GET_CVAL(v, t, x)        \
public:                             \
    [[nodiscard]] t get_##x() const \
    {                               \
        return x;                   \
    }                               \
    v:                              \
    const t x
#define GX_GET_CVAL_PRT(t, x) GX_GET_CVAL(protected, t, x)
#define GX_GET_CVAL_PRV(t, x) GX_GET_CVAL(private, t, x)
#define GX_GET_VAL(v, t, x, d)      \
public:                             \
    [[nodiscard]] t get_##x() const \
    {                               \
        return x;                   \
    }                               \
    v:                              \
    t x = d
#define GX_GETSET_VAL(v, t, x, d) \
public:                           \
    void set_##x(const t _##x)    \
    {                             \
        x = _##x;                 \
    }                             \
    GX_GET_VAL(v, t, x, d)
#define GX_GET_VAL_PRT(t, x, d) GX_GET_VAL(protected, t, x, d)
#define GX_GET_VAL_PRV(t, x, d) GX_GET_VAL(private, t, x, d)
#define GX_GETSET_VAL_PRT(t, x, d) GX_GETSET_VAL(protected, t, x, d)
#define GX_GETSET_VAL_PRV(t, x, d) GX_GETSET_VAL(private, t, x, d)
#define GX_GET_AVAL(v, t, x, d)     \
public:                             \
    [[nodiscard]] t get_##x() const \
    {                               \
        return x;                   \
    }                               \
    v:                              \
    std::atomic<t> x = d
#define GX_GET_AVAL_PRT(t, x, d) GX_GET_AVAL(protected, t, x, d)
#define GX_GETSET_AVAL(v, t, x, d) \
public:                            \
    void set_##x(const t _##x)     \
    {                              \
        x = _##x;                  \
    }                              \
    GX_GET_AVAL(v, t, x, d)
#define GX_GETSET_AVAL_PRT(t, x, d) GX_GETSET_AVAL(protected, t, x, d)
#define GX_GETSET_AVAL_PRV(t, x, d) GX_GETSET_AVAL(private, t, x, d)
#define GX_GET_CRRF(v, c, t, x)            \
public:                                    \
    [[nodiscard]] const t& get_##x() const \
    {                                      \
        return x;                          \
    }                                      \
    v:                                     \
    c t& x
#define GX_GET_RRF(v, t, x)    \
public:                        \
    [[nodiscard]] t& get_##x() \
    {                          \
        return x;              \
    }                          \
    GX_GET_CRRF(v, , t, x)
#define GX_GET_CRRF_PRT(t, x) GX_GET_CRRF(protected, const, t, x)
#define GX_GET_CRRF_PRV(t, x) GX_GET_CRRF(private, const, t, x)
#define GX_GET_RRF_PRT(t, x) GX_GET_RRF(protected, t, x)
#define GX_GET_RRF_PRV(t, x) GX_GET_RRF(private, t, x)
#endif